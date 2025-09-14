#include "class_parser.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <iomanip>

static std::string access_flags_to_string(uint16_t flags, bool is_method) {
    std::string result;
    if (flags & ClassParser::ACC_PUBLIC) result += "public ";
    if (flags & ClassParser::ACC_PRIVATE) result += "private ";
    if (flags & ClassParser::ACC_PROTECTED) result += "protected ";
    if (flags & ClassParser::ACC_STATIC) result += "static ";
    if (flags & ClassParser::ACC_FINAL) result += "final ";
    if (flags & ClassParser::ACC_SYNCHRONIZED) result += "synchronized ";
    if (flags & ClassParser::ACC_VOLATILE) result += "volatile ";
    if (flags & ClassParser::ACC_TRANSIENT) result += "transient ";
    if (flags & ClassParser::ACC_NATIVE) result += "native ";
    if (flags & ClassParser::ACC_ABSTRACT) result += "abstract ";
    if (flags & ClassParser::ACC_STRICT) result += "strictfp ";
    if (flags & ClassParser::ACC_SYNTHETIC) result += "synthetic ";
    if (flags & ClassParser::ACC_BRIDGE) result += "bridge ";
    if (flags & ClassParser::ACC_VARARGS) result += "varargs ";
    if (flags & ClassParser::ACC_ENUM) result += "enum ";
    if (flags & ClassParser::ACC_MANDATED) result += "mandated ";
    return result.empty() ? result : result.substr(0, result.size() - 1);
}

std::string ClassParser::ConstantPoolInfo::to_string() const {
    std::ostringstream oss;
    oss << "CP#" << static_cast<int>(tag) << ": ";
    switch (tag) {
        case CONSTANT_Utf8: oss << "Utf8='" << s_val << "'";
            break;
        case CONSTANT_Integer: oss << "Integer=" << i_val;
            break;
        case CONSTANT_Float: oss << "Float=" << *reinterpret_cast<const float *>(&i_val);
            break;
        case CONSTANT_Long: oss << "Long=" << l_val;
            break;
        case CONSTANT_Double: oss << "Double=" << *reinterpret_cast<const double *>(&l_val);
            break;
        case CONSTANT_Class: oss << "Class #" << index1;
            break;
        case CONSTANT_String: oss << "String #" << index1;
            break;
        case CONSTANT_Fieldref: oss << "FieldRef #" << index1 << ".#" << index2;
            break;
        case CONSTANT_Methodref: oss << "MethodRef #" << index1 << ".#" << index2;
            break;
        case CONSTANT_InterfaceMethodref: oss << "InterfaceMethodRef #" << index1 << ".#" << index2;
            break;
        case CONSTANT_NameAndType: oss << "NameAndType #" << index1 << ":#" << index2;
            break;
        case CONSTANT_MethodHandle: oss << "MethodHandle kind=" << static_cast<int>(reference_kind) << " ref=" <<
                                    index2;
            break;
        case CONSTANT_MethodType: oss << "MethodType #" << index1;
            break;
        case CONSTANT_Dynamic: oss << "Dynamic bsm=" << index1 << " name_type=" << index2;
            break;
        case CONSTANT_InvokeDynamic: oss << "InvokeDynamic bsm=" << index1 << " name_type=" << index2;
            break;
        case CONSTANT_Module: oss << "Module #" << index1;
            break;
        case CONSTANT_Package: oss << "Package #" << index1;
            break;
        default: oss << "Unknown(" << static_cast<int>(tag) << ")";
    }
    return oss.str();
}

std::string ClassParser::ExceptionTableEntry::to_string() const {
    std::ostringstream oss;
    oss << "try[" << start_pc << "-" << end_pc << "] -> handler@" << handler_pc;
    if (catch_type != 0) oss << " catch_type#" << catch_type;
    return oss.str();
}

std::string ClassParser::CodeAttribute::AttributeInfo::to_string() const {
    return name + " (" + std::to_string(info.size()) + " bytes)";
}

std::string ClassParser::CodeAttribute::to_string() const {
    std::ostringstream oss;
    oss << "Code[max_stack=" << max_stack << ", max_locals=" << max_locals
            << ", code_size=" << code.size() << " bytes, exceptions=" << exception_table.size()
            << ", attributes=" << attributes.size() << "]";
    return oss.str();
}

std::string ClassParser::SourceFileAttribute::to_string() const {
    return "SourceFile #" + std::to_string(sourcefile_index);
}

std::string ClassParser::LineNumberTableAttribute::LineNumberEntry::to_string() const {
    return "pc=" + std::to_string(start_pc) + " -> line=" + std::to_string(line_number);
}

std::string ClassParser::LineNumberTableAttribute::to_string() const {
    return "LineNumberTable[" + std::to_string(line_number_table.size()) + " entries]";
}

std::string ClassParser::LocalVariableTableAttribute::LocalVariableEntry::to_string() const {
    return "pc[" + std::to_string(start_pc) + "+" + std::to_string(length) +
           "] name#" + std::to_string(name_index) + " desc#" +
           std::to_string(descriptor_index) + " index=" + std::to_string(index);
}

std::string ClassParser::LocalVariableTableAttribute::to_string() const {
    return "LocalVariableTable[" + std::to_string(local_variable_table.size()) + " entries]";
}

std::string ClassParser::ExceptionsAttribute::to_string() const {
    return "Exceptions[" + std::to_string(exception_index_table.size()) + " exceptions]";
}

std::string ClassParser::ConstantValueAttribute::to_string() const {
    return "ConstantValue #" + std::to_string(constantvalue_index);
}

std::string ClassParser::BootstrapMethodsAttribute::BootstrapMethod::to_string() const {
    return "BSM #" + std::to_string(bootstrap_method_ref) +
           " args[" + std::to_string(bootstrap_arguments.size()) + "]";
}

std::string ClassParser::BootstrapMethodsAttribute::to_string() const {
    return "BootstrapMethods[" + std::to_string(bootstrap_methods.size()) + " methods]";
}

std::string ClassParser::SignatureAttribute::to_string() const {
    return "Signature #" + std::to_string(signature_index);
}

std::string ClassParser::DeprecatedAttribute::to_string() const {
    return "Deprecated";
}

std::string ClassParser::SyntheticAttribute::to_string() const {
    return "Synthetic";
}

std::string ClassParser::InnerClassesAttribute::InnerClass::to_string() const {
    return "InnerClass[inner#" + std::to_string(inner_class_info_index) +
           ", outer#" + std::to_string(outer_class_info_index) +
           ", name#" + std::to_string(inner_name_index) +
           ", flags=" + std::to_string(inner_class_access_flags) + "]";
}

std::string ClassParser::InnerClassesAttribute::to_string() const {
    return "InnerClasses[" + std::to_string(classes.size()) + " classes]";
}

std::string ClassParser::EnclosingMethodAttribute::to_string() const {
    return "EnclosingMethod[class#" + std::to_string(class_index) +
           ", method#" + std::to_string(method_index) + "]";
}

std::string ClassParser::SourceDebugExtensionAttribute::to_string() const {
    return "SourceDebugExtension[" + std::to_string(debug_extension.size()) + " bytes]";
}

std::string ClassParser::LocalVariableTypeTableAttribute::LocalVariableTypeEntry::to_string() const {
    return "pc[" + std::to_string(start_pc) + "+" + std::to_string(length) +
           "] name#" + std::to_string(name_index) + " sig#" +
           std::to_string(signature_index) + " index=" + std::to_string(index);
}

std::string ClassParser::LocalVariableTypeTableAttribute::to_string() const {
    return "LocalVariableTypeTable[" + std::to_string(local_variable_type_table.size()) + " entries]";
}

std::string ClassParser::MethodParametersAttribute::Parameter::to_string() const {
    return "Parameter[name#" + std::to_string(name_index) +
           ", flags=" + std::to_string(access_flags) + "]";
}

std::string ClassParser::MethodParametersAttribute::to_string() const {
    return "MethodParameters[" + std::to_string(parameters.size()) + " parameters]";
}

std::string ClassParser::RuntimeVisibleAnnotationsAttribute::to_string() const {
    return "RuntimeVisibleAnnotations[" + std::to_string(num_annotations) + " annotations]";
}

std::string ClassParser::RuntimeInvisibleAnnotationsAttribute::to_string() const {
    return "RuntimeInvisibleAnnotations[" + std::to_string(num_annotations) + " annotations]";
}

std::string ClassParser::RuntimeVisibleParameterAnnotationsAttribute::to_string() const {
    return "RuntimeVisibleParameterAnnotations[" + std::to_string(num_parameters) + " parameters]";
}

std::string ClassParser::RuntimeInvisibleParameterAnnotationsAttribute::to_string() const {
    return "RuntimeInvisibleParameterAnnotations[" + std::to_string(num_parameters) + " parameters]";
}

std::string ClassParser::RuntimeVisibleTypeAnnotationsAttribute::to_string() const {
    return "RuntimeVisibleTypeAnnotations[" + std::to_string(num_annotations) + " annotations]";
}

std::string ClassParser::RuntimeInvisibleTypeAnnotationsAttribute::to_string() const {
    return "RuntimeInvisibleTypeAnnotations[" + std::to_string(num_annotations) + " annotations]";
}

std::string ClassParser::AnnotationDefaultAttribute::to_string() const {
    return "AnnotationDefault[" + std::to_string(default_value.size()) + " bytes]";
}

std::string ClassParser::ModuleAttribute::RequiresEntry::to_string() const {
    return "Requires[index#" + std::to_string(index) +
           ", flags=" + std::to_string(flags) +
           ", version#" + std::to_string(version_index) + "]";
}

std::string ClassParser::ModuleAttribute::ExportsEntry::to_string() const {
    return "Exports[index#" + std::to_string(index) +
           ", flags=" + std::to_string(flags) +
           ", to_count=" + std::to_string(to_indices.size()) + "]";
}

std::string ClassParser::ModuleAttribute::OpensEntry::to_string() const {
    return "Opens[index#" + std::to_string(index) +
           ", flags=" + std::to_string(flags) +
           ", to_count=" + std::to_string(to_indices.size()) + "]";
}

std::string ClassParser::ModuleAttribute::ProvidesEntry::to_string() const {
    return "Provides[index#" + std::to_string(index) +
           ", with_count=" + std::to_string(with_indices.size()) + "]";
}

std::string ClassParser::ModuleAttribute::to_string() const {
    return "Module[name#" + std::to_string(module_name_index) +
           ", flags=" + std::to_string(module_flags) +
           ", version#" + std::to_string(module_version_index) + "]";
}

std::string ClassParser::ModulePackagesAttribute::to_string() const {
    return "ModulePackages[" + std::to_string(package_index.size()) + " packages]";
}

std::string ClassParser::ModuleMainClassAttribute::to_string() const {
    return "ModuleMainClass #" + std::to_string(main_class_index);
}

std::string ClassParser::NestHostAttribute::to_string() const {
    return "NestHost #" + std::to_string(host_class_index);
}

std::string ClassParser::NestMembersAttribute::to_string() const {
    return "NestMembers[" + std::to_string(classes.size()) + " classes]";
}

std::string ClassParser::RecordAttribute::ComponentInfo::to_string() const {
    return "Component[name#" + std::to_string(name_index) +
           ", desc#" + std::to_string(descriptor_index) +
           ", attrs=" + std::to_string(attributes.size()) + "]";
}

std::string ClassParser::RecordAttribute::to_string() const {
    return "Record[" + std::to_string(components.size()) + " components]";
}

std::string ClassParser::PermittedSubclassesAttribute::to_string() const {
    return "PermittedSubclasses[" + std::to_string(classes.size()) + " classes]";
}

std::string ClassParser::SpecializedAttribute::to_string() const {
    switch (type) {
        case SOURCE_FILE: return source_file.to_string();
        case LINE_NUMBER_TABLE: return line_number_table.to_string();
        case LOCAL_VARIABLE_TABLE: return local_variable_table.to_string();
        case EXCEPTIONS: return exceptions.to_string();
        case CONSTANT_VALUE: return constant_value.to_string();
        case BOOTSTRAP_METHODS: return bootstrap_methods.to_string();
        case SIGNATURE: return signature.to_string();
        case DEPRECATED: return deprecated.to_string();
        case SYNTHETIC: return synthetic.to_string();
        case INNER_CLASSES: return inner_classes.to_string();
        case ENCLOSING_METHOD: return enclosing_method.to_string();
        case SOURCE_DEBUG_EXTENSION: return source_debug_extension.to_string();
        case LOCAL_VARIABLE_TYPE_TABLE: return local_variable_type_table.to_string();
        case METHOD_PARAMETERS: return method_parameters.to_string();
        case RUNTIME_VISIBLE_ANNOTATIONS: return runtime_visible_annotations.to_string();
        case RUNTIME_INVISIBLE_ANNOTATIONS: return runtime_invisible_annotations.to_string();
        case RUNTIME_VISIBLE_PARAMETER_ANNOTATIONS: return runtime_visible_parameter_annotations.to_string();
        case RUNTIME_INVISIBLE_PARAMETER_ANNOTATIONS: return runtime_invisible_parameter_annotations.to_string();
        case RUNTIME_VISIBLE_TYPE_ANNOTATIONS: return runtime_visible_type_annotations.to_string();
        case RUNTIME_INVISIBLE_TYPE_ANNOTATIONS: return runtime_invisible_type_annotations.to_string();
        case ANNOTATION_DEFAULT: return annotation_default.to_string();
        case MODULE: return module_attr.to_string();
        case MODULE_PACKAGES: return module_packages.to_string();
        case MODULE_MAIN_CLASS: return module_main_class.to_string();
        case NEST_HOST: return nest_host.to_string();
        case NEST_MEMBERS: return nest_members.to_string();
        case RECORD: return record.to_string();
        case PERMITTED_SUBCLASSES: return permitted_subclasses.to_string();
        default: return "UnknownAttribute[" + name + ", " + std::to_string(raw_data.size()) + " bytes]";
    }
}

ClassParser::MethodInfo::~MethodInfo() {
    delete code_attribute;
}

std::string ClassParser::MethodInfo::to_string() const {
    std::ostringstream oss;
    oss << "Method[" << access_flags_to_string(access_flags, true) << " "
            << name << descriptor;
    if (code_attribute) oss << " " << code_attribute->to_string();
    oss << ", attributes=" << attributes.size() << "]";
    return oss.str();
}

std::string ClassParser::FieldInfo::to_string() const {
    return "Field[" + access_flags_to_string(access_flags, false) + " " +
           name + " " + descriptor +
           ", attributes=" + std::to_string(attributes.size()) + "]";
}

ClassParser::ClassParser(const std::string &filename) : filename(filename), file_data(nullptr) {
    if (!load_file()) {
        throw std::runtime_error("Failed to load file: " + filename);
    }
}

ClassParser::~ClassParser() {
    for (auto p: constant_pool) {
        delete p;
    }
    constant_pool.clear();

    if (file_data) {
        delete[] file_data;
        file_data = nullptr;
    }
}

bool ClassParser::load_file() {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }
    file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    file_data = new uint8_t[file_size];
    file.read(reinterpret_cast<char *>(file_data), file_size);
    file.close();
    return true;
}

void ClassParser::ensure_available(const size_t bytes) const {
    if (cursor + bytes > file_size) {
        throw std::runtime_error("Reading outside file boundaries (need " +
                                 std::to_string(bytes) + " bytes, have " +
                                 std::to_string(file_size - cursor) + ")");
    }
}

uint8_t ClassParser::read_uint8() {
    ensure_available(1);
    return file_data[cursor++];
}

uint16_t ClassParser::read_uint16() {
    ensure_available(2);
    const uint16_t value = (static_cast<uint16_t>(file_data[cursor]) << 8) | file_data[cursor + 1];
    cursor += 2;
    return value;
}

uint32_t ClassParser::read_uint32() {
    ensure_available(4);
    const uint32_t value = (static_cast<uint32_t>(file_data[cursor]) << 24) |
                           (static_cast<uint32_t>(file_data[cursor + 1]) << 16) |
                           (static_cast<uint32_t>(file_data[cursor + 2]) << 8) |
                           file_data[cursor + 3];
    cursor += 4;
    return value;
}

uint64_t ClassParser::read_uint64() {
    ensure_available(8);
    const uint64_t value = (static_cast<uint64_t>(file_data[cursor]) << 56) |
                           (static_cast<uint64_t>(file_data[cursor + 1]) << 48) |
                           (static_cast<uint64_t>(file_data[cursor + 2]) << 40) |
                           (static_cast<uint64_t>(file_data[cursor + 3]) << 32) |
                           (static_cast<uint64_t>(file_data[cursor + 4]) << 24) |
                           (static_cast<uint64_t>(file_data[cursor + 5]) << 16) |
                           (static_cast<uint64_t>(file_data[cursor + 6]) << 8) |
                           file_data[cursor + 7];
    cursor += 8;
    return value;
}

std::string ClassParser::read_modified_utf8(uint16_t length) {
    ensure_available(length);
    std::string result;
    result.reserve(length);

    for (uint16_t i = 0; i < length; ++i) {
        if (const uint8_t b1 = file_data[cursor + i]; b1 == 0) {
            result += '\0';
        } else if ((b1 & 0x80) == 0) {
            result += static_cast<char>(b1);
        } else if ((b1 & 0xE0) == 0xC0) {
            if (i + 1 >= length) break;
            const uint8_t b2 = file_data[cursor + i + 1];
            if ((b2 & 0xC0) != 0x80) break;

            const uint16_t code_point = ((b1 & 0x1F) << 6) | (b2 & 0x3F);
            result += static_cast<char>(code_point);
            i += 1;
        } else if ((b1 & 0xF0) == 0xE0) {
            if (i + 2 >= length) break;
            const uint8_t b2 = file_data[cursor + i + 1];
            const uint8_t b3 = file_data[cursor + i + 2];
            if ((b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) break;

            const uint16_t code_point = ((b1 & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
            result += static_cast<char>(code_point);
            i += 2;
        } else {
            result += '?';
        }
    }

    cursor += length;
    return result;
}

void ClassParser::parse() {
    cursor = 0;

    magic = read_uint32();
    if (magic != 0xCAFEBABE) {
        throw std::runtime_error("Invalid magic number: 0x" + std::to_string(magic));
    }

    minor_version = read_uint16();
    major_version = read_uint16();

    if (major_version > 65) {
        std::cout << "Warning: This class file version (" << major_version
                << ") may contain features not fully supported" << std::endl;
    }

    parse_constant_pool();

    access_flags = read_uint16();
    this_class_index = read_uint16();
    super_class_index = read_uint16();

    class_name = get_class_name(this_class_index);
    super_class_name = get_super_class_name(super_class_index);

    parse_interfaces();
    parse_fields();
    parse_methods();

    class_attributes.clear();
    parse_attributes(read_uint16(), nullptr, nullptr, &class_attributes);
}

void ClassParser::parse_constant_pool() {
    const uint16_t cp_count = read_uint16();
    constant_pool.resize(cp_count, nullptr);

    for (int i = 1; i < cp_count; ++i) {
        const uint8_t tag = read_uint8();
        ConstantPoolInfo *info = new ConstantPoolInfo();
        info->tag = tag;

        switch (tag) {
            case CONSTANT_Utf8: {
                const uint16_t length = read_uint16();
                info->s_val = read_modified_utf8(length);
                break;
            }
            case CONSTANT_Integer:
                info->i_val = read_uint32();
                break;
            case CONSTANT_Float:
                info->i_val = read_uint32();
                break;
            case CONSTANT_Long:
                info->l_val = read_uint64();
                i++;
                break;
            case CONSTANT_Double:
                info->l_val = read_uint64();
                i++;
                break;
            case CONSTANT_Class:
            case CONSTANT_String:
                info->index1 = read_uint16();
                break;
            case CONSTANT_Fieldref:
            case CONSTANT_Methodref:
            case CONSTANT_InterfaceMethodref:
            case CONSTANT_NameAndType:
                info->index1 = read_uint16();
                info->index2 = read_uint16();
                break;
            case CONSTANT_MethodHandle:
                info->reference_kind = read_uint8();
                info->index2 = read_uint16();
                break;
            case CONSTANT_MethodType:
                info->index1 = read_uint16();
                break;
            case CONSTANT_InvokeDynamic:
            case CONSTANT_Dynamic:
                info->index1 = read_uint16();
                info->index2 = read_uint16();
                break;
            case CONSTANT_Module:
            case CONSTANT_Package:
                info->index1 = read_uint16();
                break;
            default:
                delete info;
                throw std::runtime_error("Unsupported constant pool tag: " +
                                         std::to_string(tag));
        }
        constant_pool[i] = info;
    }
}

void ClassParser::parse_interfaces() {
    interfaces_count = read_uint16();
    interfaces.resize(interfaces_count);
    for (uint16_t i = 0; i < interfaces_count; ++i) {
        interfaces[i] = read_uint16();
    }
}

void ClassParser::parse_fields() {
    const uint16_t fields_count = read_uint16();
    fields.resize(fields_count);
    for (int i = 0; i < fields_count; ++i) {
        fields[i].access_flags = read_uint16();
        fields[i].name_index = read_uint16();
        fields[i].descriptor_index = read_uint16();
        fields[i].attributes_count = read_uint16();

        fields[i].name = get_utf8_string(fields[i].name_index);
        fields[i].descriptor = get_utf8_string(fields[i].descriptor_index);

        fields[i].attributes.clear();
        parse_attributes(fields[i].attributes_count, nullptr, &fields[i],
                         &fields[i].attributes);
    }
}

void ClassParser::parse_methods() {
    const uint16_t methods_count = read_uint16();
    methods.resize(methods_count);
    for (int i = 0; i < methods_count; ++i) {
        methods[i].access_flags = read_uint16();
        methods[i].name_index = read_uint16();
        methods[i].descriptor_index = read_uint16();
        methods[i].attributes_count = read_uint16();

        methods[i].name = get_utf8_string(methods[i].name_index);
        methods[i].descriptor = get_utf8_string(methods[i].descriptor_index);

        methods[i].attributes.clear();
        methods[i].code_attribute = nullptr;
        parse_attributes(methods[i].attributes_count, &methods[i], nullptr,
                         &methods[i].attributes);
    }
}

void ClassParser::parse_attributes(const uint16_t count, MethodInfo *method,
                                   FieldInfo *field,
                                   std::vector<CodeAttribute::AttributeInfo> *out_attrs) {
    for (int i = 0; i < count; ++i) {
        const uint16_t attribute_name_index = read_uint16();
        const uint32_t attribute_length = read_uint32();

        std::string attribute_name = get_utf8_string(attribute_name_index);

        if (attribute_name == "Code" && method != nullptr) {
            CodeAttribute *code_attr = new CodeAttribute();
            code_attr->max_stack = read_uint16();
            code_attr->max_locals = read_uint16();

            uint32_t code_length = read_uint32();
            ensure_available(code_length);
            code_attr->code.resize(code_length);
            memcpy(code_attr->code.data(), file_data + cursor, code_length);
            cursor += code_length;

            const uint16_t exception_table_length = read_uint16();
            code_attr->exception_table.resize(exception_table_length);
            for (uint16_t e = 0; e < exception_table_length; ++e) {
                code_attr->exception_table[e].start_pc = read_uint16();
                code_attr->exception_table[e].end_pc = read_uint16();
                code_attr->exception_table[e].handler_pc = read_uint16();
                code_attr->exception_table[e].catch_type = read_uint16();
            }

            const uint16_t code_attributes_count = read_uint16();
            code_attr->attributes.clear();
            for (uint16_t a = 0; a < code_attributes_count; ++a) {
                const uint16_t ca_name_index = read_uint16();
                const uint32_t ca_len = read_uint32();
                const std::string ca_name = get_utf8_string(ca_name_index);

                CodeAttribute::AttributeInfo ai;
                ai.name = ca_name;
                ai.info.resize(ca_len);
                ensure_available(ca_len);
                memcpy(ai.info.data(), file_data + cursor, ca_len);
                cursor += ca_len;
                code_attr->attributes.push_back(std::move(ai));
            }

            method->code_attribute = code_attr;
        } else {
            CodeAttribute::AttributeInfo ai;
            ai.name = attribute_name;
            ai.info.resize(attribute_length);
            ensure_available(attribute_length);
            memcpy(ai.info.data(), file_data + cursor, attribute_length);
            cursor += attribute_length;

            if (out_attrs != nullptr) {
                out_attrs->push_back(std::move(ai));
            }
        }
    }
}

ClassParser::MethodInfo *ClassParser::find_main_method() {
    return find_method("main", "([Ljava/lang/String;)V");
}

ClassParser::MethodInfo *ClassParser::find_method(const std::string &name) {
    for (MethodInfo &m: methods) {
        if (m.name == name) {
            return &m;
        }
    }
    return nullptr;
}

ClassParser::MethodInfo *ClassParser::find_method(const std::string &name, const std::string &descriptor) {
    for (MethodInfo &m: methods) {
        if (m.name == name && m.descriptor == descriptor) {
            return &m;
        }
    }
    return nullptr;
}

const std::vector<ClassParser::ConstantPoolInfo *> &ClassParser::get_constant_pool() const {
    return constant_pool;
}

const std::string &ClassParser::get_class_name() const {
    return class_name;
}

const std::string &ClassParser::get_super_class_name() const {
    return super_class_name;
}

std::string ClassParser::get_utf8_string(const uint16_t index) const {
    if (index == 0 || index >= constant_pool.size() ||
        constant_pool[index] == nullptr ||
        constant_pool[index]->tag != CONSTANT_Utf8) {
        throw std::runtime_error("Invalid Utf8 index in constant pool: " +
                                 std::to_string(index));
    }
    return constant_pool[index]->s_val;
}

std::string ClassParser::get_class_name(const uint16_t index) const {
    if (index == 0 || index >= constant_pool.size() ||
        constant_pool[index] == nullptr ||
        constant_pool[index]->tag != CONSTANT_Class) {
        throw std::runtime_error("Invalid Class index in constant pool: " +
                                 std::to_string(index));
    }
    return get_utf8_string(constant_pool[index]->index1);
}

std::string ClassParser::get_super_class_name(const uint16_t index) const {
    if (index == 0) {
        return "java/lang/Object";
    }
    return get_class_name(index);
}

std::string ClassParser::get_method_descriptor(const uint16_t index) const {
    return get_utf8_string(index);
}

std::string ClassParser::get_field_descriptor(const uint16_t index) const {
    return get_utf8_string(index);
}

std::string ClassParser::get_method_name(const MethodInfo &method) const {
    return method.name;
}

std::string ClassParser::get_field_name(const FieldInfo &field) const {
    return field.name;
}

std::string ClassParser::get_access_flags_string(const uint16_t flags, const bool is_method) const {
    return access_flags_to_string(flags, is_method);
}

std::string ClassParser::to_string() const {
    std::ostringstream oss;
    oss << "Class: " << class_name << "\n";
    oss << "Version: " << major_version << "." << minor_version << "\n";
    oss << "Access flags: " << get_access_flags_string(access_flags, false) << "\n";
    oss << "Super class: " << (super_class_index ? get_class_name(super_class_index) : "java/lang/Object") << "\n";
    oss << "Interfaces: " << interfaces.size() << "\n";
    oss << "Fields: " << fields.size() << "\n";
    for (const auto &field: fields) {
        oss << "  " << field.to_string() << "\n";
    }
    oss << "Methods: " << methods.size() << "\n";
    for (const auto &method: methods) {
        oss << "  " << method.to_string() << "\n";
    }
    oss << "Constant pool entries: " << constant_pool.size() - 1 << "\n";
    return oss.str();
}

void ClassParser::parse_source_file_attribute(SourceFileAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        attr.sourcefile_index = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    }
}

ClassParser::SpecializedAttribute ClassParser::parse_specialized_attribute(
    const std::string &name, const std::vector<uint8_t> &data) {
    SpecializedAttribute attr;
    attr.name = name;
    attr.raw_data = data;

    if (name == "SourceFile") {
        attr.type = SpecializedAttribute::SOURCE_FILE;
        parse_source_file_attribute(attr.source_file, data);
    } else if (name == "LineNumberTable") {
        attr.type = SpecializedAttribute::LINE_NUMBER_TABLE;
        parse_line_number_table_attribute(attr.line_number_table, data);
    } else if (name == "LocalVariableTable") {
        attr.type = SpecializedAttribute::LOCAL_VARIABLE_TABLE;
        parse_local_variable_table_attribute(attr.local_variable_table, data);
    } else if (name == "Exceptions") {
        attr.type = SpecializedAttribute::EXCEPTIONS;
        parse_exceptions_attribute(attr.exceptions, data);
    } else if (name == "ConstantValue") {
        attr.type = SpecializedAttribute::CONSTANT_VALUE;
        parse_constant_value_attribute(attr.constant_value, data);
    } else if (name == "BootstrapMethods") {
        attr.type = SpecializedAttribute::BOOTSTRAP_METHODS;
        parse_bootstrap_methods_attribute(attr.bootstrap_methods, data);
    } else if (name == "Signature") {
        attr.type = SpecializedAttribute::SIGNATURE;
        parse_signature_attribute(attr.signature, data);
    } else if (name == "Deprecated") {
        attr.type = SpecializedAttribute::DEPRECATED;
        parse_deprecated_attribute(attr.deprecated, data);
    } else if (name == "Synthetic") {
        attr.type = SpecializedAttribute::SYNTHETIC;
        parse_synthetic_attribute(attr.synthetic, data);
    } else if (name == "InnerClasses") {
        attr.type = SpecializedAttribute::INNER_CLASSES;
        parse_inner_classes_attribute(attr.inner_classes, data);
    } else if (name == "EnclosingMethod") {
        attr.type = SpecializedAttribute::ENCLOSING_METHOD;
        parse_enclosing_method_attribute(attr.enclosing_method, data);
    } else if (name == "SourceDebugExtension") {
        attr.type = SpecializedAttribute::SOURCE_DEBUG_EXTENSION;
        parse_source_debug_extension_attribute(attr.source_debug_extension, data);
    } else if (name == "LocalVariableTypeTable") {
        attr.type = SpecializedAttribute::LOCAL_VARIABLE_TYPE_TABLE;
        parse_local_variable_type_table_attribute(attr.local_variable_type_table, data);
    } else if (name == "MethodParameters") {
        attr.type = SpecializedAttribute::METHOD_PARAMETERS;
        parse_method_parameters_attribute(attr.method_parameters, data);
    } else if (name == "RuntimeVisibleAnnotations") {
        attr.type = SpecializedAttribute::RUNTIME_VISIBLE_ANNOTATIONS;
        parse_runtime_visible_annotations_attribute(attr.runtime_visible_annotations, data);
    } else if (name == "RuntimeInvisibleAnnotations") {
        attr.type = SpecializedAttribute::RUNTIME_INVISIBLE_ANNOTATIONS;
        parse_runtime_invisible_annotations_attribute(attr.runtime_invisible_annotations, data);
    } else if (name == "RuntimeVisibleParameterAnnotations") {
        attr.type = SpecializedAttribute::RUNTIME_VISIBLE_PARAMETER_ANNOTATIONS;
        parse_runtime_visible_parameter_annotations_attribute(attr.runtime_visible_parameter_annotations, data);
    } else if (name == "RuntimeInvisibleParameterAnnotations") {
        attr.type = SpecializedAttribute::RUNTIME_INVISIBLE_PARAMETER_ANNOTATIONS;
        parse_runtime_invisible_parameter_annotations_attribute(attr.runtime_invisible_parameter_annotations, data);
    } else if (name == "RuntimeVisibleTypeAnnotations") {
        attr.type = SpecializedAttribute::RUNTIME_VISIBLE_TYPE_ANNOTATIONS;
        parse_runtime_visible_type_annotations_attribute(attr.runtime_visible_type_annotations, data);
    } else if (name == "RuntimeInvisibleTypeAnnotations") {
        attr.type = SpecializedAttribute::RUNTIME_INVISIBLE_TYPE_ANNOTATIONS;
        parse_runtime_invisible_type_annotations_attribute(attr.runtime_invisible_type_annotations, data);
    } else if (name == "AnnotationDefault") {
        attr.type = SpecializedAttribute::ANNOTATION_DEFAULT;
        parse_annotation_default_attribute(attr.annotation_default, data);
    } else if (name == "Module") {
        attr.type = SpecializedAttribute::MODULE;
        parse_module_attribute(attr.module_attr, data);
    } else if (name == "ModulePackages") {
        attr.type = SpecializedAttribute::MODULE_PACKAGES;
        parse_module_packages_attribute(attr.module_packages, data);
    } else if (name == "ModuleMainClass") {
        attr.type = SpecializedAttribute::MODULE_MAIN_CLASS;
        parse_module_main_class_attribute(attr.module_main_class, data);
    } else if (name == "NestHost") {
        attr.type = SpecializedAttribute::NEST_HOST;
        parse_nest_host_attribute(attr.nest_host, data);
    } else if (name == "NestMembers") {
        attr.type = SpecializedAttribute::NEST_MEMBERS;
        parse_nest_members_attribute(attr.nest_members, data);
    } else if (name == "Record") {
        attr.type = SpecializedAttribute::RECORD;
        parse_record_attribute(attr.record, data);
    } else if (name == "PermittedSubclasses") {
        attr.type = SpecializedAttribute::PERMITTED_SUBCLASSES;
        parse_permitted_subclasses_attribute(attr.permitted_subclasses, data);
    } else {
        attr.type = SpecializedAttribute::UNKNOWN;
    }

    return attr;
}

void ClassParser::parse_line_number_table_attribute(LineNumberTableAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        const uint16_t line_number_table_length = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        attr.line_number_table.resize(line_number_table_length);

        size_t offset = 2;
        for (uint16_t i = 0; i < line_number_table_length && offset + 4 <= data.size(); ++i) {
            attr.line_number_table[i].start_pc = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            attr.line_number_table[i].line_number = (static_cast<uint16_t>(data[offset + 2]) << 8) | data[offset + 3];
            offset += 4;
        }
    }
}

void ClassParser::parse_local_variable_table_attribute(LocalVariableTableAttribute &attr,
                                                       const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        const uint16_t local_variable_table_length = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        attr.local_variable_table.resize(local_variable_table_length);

        size_t offset = 2;
        for (uint16_t i = 0; i < local_variable_table_length && offset + 10 <= data.size(); ++i) {
            attr.local_variable_table[i].start_pc = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            attr.local_variable_table[i].length = (static_cast<uint16_t>(data[offset + 2]) << 8) | data[offset + 3];
            attr.local_variable_table[i].name_index = (static_cast<uint16_t>(data[offset + 4]) << 8) | data[offset + 5];
            attr.local_variable_table[i].descriptor_index =
                    (static_cast<uint16_t>(data[offset + 6]) << 8) | data[offset + 7];
            attr.local_variable_table[i].index = (static_cast<uint16_t>(data[offset + 8]) << 8) | data[offset + 9];
            offset += 10;
        }
    }
}

void ClassParser::parse_exceptions_attribute(ExceptionsAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        const uint16_t number_of_exceptions = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        attr.exception_index_table.resize(number_of_exceptions);

        size_t offset = 2;
        for (uint16_t i = 0; i < number_of_exceptions && offset + 2 <= data.size(); ++i) {
            attr.exception_index_table[i] = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            offset += 2;
        }
    }
}

void ClassParser::parse_constant_value_attribute(ConstantValueAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        attr.constantvalue_index = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    }
}

void ClassParser::parse_bootstrap_methods_attribute(BootstrapMethodsAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        const uint16_t num_bootstrap_methods = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        attr.bootstrap_methods.resize(num_bootstrap_methods);

        size_t offset = 2;
        for (uint16_t i = 0; i < num_bootstrap_methods && offset + 4 <= data.size(); ++i) {
            attr.bootstrap_methods[i].bootstrap_method_ref =
                    (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            const uint16_t num_bootstrap_arguments = (static_cast<uint16_t>(data[offset + 2]) << 8) | data[offset + 3];
            attr.bootstrap_methods[i].bootstrap_arguments.resize(num_bootstrap_arguments);
            offset += 4;

            for (uint16_t j = 0; j < num_bootstrap_arguments && offset + 2 <= data.size(); ++j) {
                attr.bootstrap_methods[i].bootstrap_arguments[j] =
                        (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
                offset += 2;
            }
        }
    }
}

void ClassParser::parse_signature_attribute(SignatureAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        attr.signature_index = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    }
}

void ClassParser::parse_deprecated_attribute(DeprecatedAttribute &attr, const std::vector<uint8_t> &data) {
}

void ClassParser::parse_synthetic_attribute(SyntheticAttribute &attr, const std::vector<uint8_t> &data) {
}

void ClassParser::parse_inner_classes_attribute(InnerClassesAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        const uint16_t number_of_classes = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        attr.classes.resize(number_of_classes);

        size_t offset = 2;
        for (uint16_t i = 0; i < number_of_classes && offset + 8 <= data.size(); ++i) {
            attr.classes[i].inner_class_info_index = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            attr.classes[i].outer_class_info_index = (static_cast<uint16_t>(data[offset + 2]) << 8) | data[offset + 3];
            attr.classes[i].inner_name_index = (static_cast<uint16_t>(data[offset + 4]) << 8) | data[offset + 5];
            attr.classes[i].inner_class_access_flags =
                    (static_cast<uint16_t>(data[offset + 6]) << 8) | data[offset + 7];
            offset += 8;
        }
    }
}

void ClassParser::parse_enclosing_method_attribute(EnclosingMethodAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 4) {
        attr.class_index = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        attr.method_index = (static_cast<uint16_t>(data[2]) << 8) | data[3];
    }
}

void ClassParser::parse_source_debug_extension_attribute(SourceDebugExtensionAttribute &attr,
                                                         const std::vector<uint8_t> &data) {
    attr.debug_extension = data;
}

void ClassParser::parse_local_variable_type_table_attribute(LocalVariableTypeTableAttribute &attr,
                                                            const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        const uint16_t local_variable_type_table_length = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        attr.local_variable_type_table.resize(local_variable_type_table_length);

        size_t offset = 2;
        for (uint16_t i = 0; i < local_variable_type_table_length && offset + 10 <= data.size(); ++i) {
            attr.local_variable_type_table[i].start_pc = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            attr.local_variable_type_table[i].length =
                    (static_cast<uint16_t>(data[offset + 2]) << 8) | data[offset + 3];
            attr.local_variable_type_table[i].name_index =
                    (static_cast<uint16_t>(data[offset + 4]) << 8) | data[offset + 5];
            attr.local_variable_type_table[i].signature_index =
                    (static_cast<uint16_t>(data[offset + 6]) << 8) | data[offset + 7];
            attr.local_variable_type_table[i].index = (static_cast<uint16_t>(data[offset + 8]) << 8) | data[offset + 9];
            offset += 10;
        }
    }
}

void ClassParser::parse_method_parameters_attribute(MethodParametersAttribute &attr, const std::vector<uint8_t> &data) {
    if (!data.empty()) {
        const uint8_t parameters_count = data[0];
        attr.parameters.resize(parameters_count);

        size_t offset = 1;
        for (uint8_t i = 0; i < parameters_count && offset + 4 <= data.size(); ++i) {
            attr.parameters[i].name_index = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            attr.parameters[i].access_flags = (static_cast<uint16_t>(data[offset + 2]) << 8) | data[offset + 3];
            offset += 4;
        }
    }
}

void ClassParser::parse_runtime_visible_annotations_attribute(RuntimeVisibleAnnotationsAttribute &attr,
                                                              const std::vector<uint8_t> &data) {
    attr.annotations = data;
    if (data.size() >= 2) {
        attr.num_annotations = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    }
}

void ClassParser::parse_runtime_invisible_annotations_attribute(RuntimeInvisibleAnnotationsAttribute &attr,
                                                                const std::vector<uint8_t> &data) {
    attr.annotations = data;
    if (data.size() >= 2) {
        attr.num_annotations = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    }
}

void ClassParser::parse_runtime_visible_parameter_annotations_attribute(
    RuntimeVisibleParameterAnnotationsAttribute &attr, const std::vector<uint8_t> &data) {
    if (!data.empty()) {
        attr.num_parameters = data[0];
        attr.parameter_annotations.resize(attr.num_parameters);
        const size_t offset = 1;
        for (uint8_t p = 0; p < attr.num_parameters; ++p) {
            if (offset + 2 > data.size()) break;
            uint16_t num = data[offset + 1];
            if (offset < data.size()) {
                attr.parameter_annotations[p] = std::vector(data.begin() + offset, data.end());
            }
            break;
        }
    }
}

void ClassParser::parse_runtime_invisible_parameter_annotations_attribute(
    RuntimeInvisibleParameterAnnotationsAttribute &attr, const std::vector<uint8_t> &data) {
    if (!data.empty()) {
        attr.num_parameters = data[0];
        attr.parameter_annotations.resize(attr.num_parameters);
        const size_t offset = 1;
        for (uint8_t p = 0; p < attr.num_parameters; ++p) {
            if (offset + 2 > data.size()) break;
            uint16_t num = data[offset + 1];
            if (offset < data.size()) {
                attr.parameter_annotations[p] = std::vector(data.begin() + offset, data.end());
            }
            break;
        }
    }
}

void ClassParser::parse_runtime_visible_type_annotations_attribute(RuntimeVisibleTypeAnnotationsAttribute &attr,
                                                                   const std::vector<uint8_t> &data) {
    attr.type_annotations = data;
    if (data.size() >= 2) {
        attr.num_annotations = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    }
}

void ClassParser::parse_runtime_invisible_type_annotations_attribute(RuntimeInvisibleTypeAnnotationsAttribute &attr,
                                                                     const std::vector<uint8_t> &data) {
    attr.type_annotations = data;
    if (data.size() >= 2) {
        attr.num_annotations = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    }
}

void ClassParser::parse_annotation_default_attribute(AnnotationDefaultAttribute &attr,
                                                     const std::vector<uint8_t> &data) {
    attr.default_value = data;
}

void ClassParser::parse_module_attribute(ModuleAttribute &attr, const std::vector<uint8_t> &data) {
    size_t offset = 0;
    auto read_u2 = [&](uint16_t &out) {
        if (offset + 2 > data.size()) return false;
        out = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
        offset += 2;
        return true;
    };
    if (!read_u2(attr.module_name_index)) return;
    if (!read_u2(attr.module_flags)) return;
    if (!read_u2(attr.module_version_index)) return;

    uint16_t requires_count = 0;
    if (!read_u2(requires_count)) return;
    attr.requires_entries.resize(requires_count);
    for (uint16_t i = 0; i < requires_count; ++i) {
        if (!read_u2(attr.requires_entries[i].index)) return;
        if (!read_u2(attr.requires_entries[i].flags)) return;
        if (!read_u2(attr.requires_entries[i].version_index)) return;
    }

    uint16_t exports_count = 0;
    if (!read_u2(exports_count)) return;
    attr.exports_entries.resize(exports_count);
    for (uint16_t i = 0; i < exports_count; ++i) {
        uint16_t to_count = 0;
        if (!read_u2(attr.exports_entries[i].index)) return;
        if (!read_u2(attr.exports_entries[i].flags)) return;
        if (!read_u2(to_count)) return;
        attr.exports_entries[i].to_indices.resize(to_count);
        for (uint16_t j = 0; j < to_count; ++j) {
            if (!read_u2(attr.exports_entries[i].to_indices[j])) return;
        }
    }

    uint16_t opens_count = 0;
    if (!read_u2(opens_count)) return;
    attr.opens_entries.resize(opens_count);
    for (uint16_t i = 0; i < opens_count; ++i) {
        uint16_t to_count = 0;
        if (!read_u2(attr.opens_entries[i].index)) return;
        if (!read_u2(attr.opens_entries[i].flags)) return;
        if (!read_u2(to_count)) return;
        attr.opens_entries[i].to_indices.resize(to_count);
        for (uint16_t j = 0; j < to_count; ++j) {
            if (!read_u2(attr.opens_entries[i].to_indices[j])) return;
        }
    }

    uint16_t uses_count = 0;
    if (!read_u2(uses_count)) return;
    attr.uses.resize(uses_count);
    for (uint16_t i = 0; i < uses_count; ++i) {
        if (!read_u2(attr.uses[i])) return;
    }

    uint16_t provides_count = 0;
    if (!read_u2(provides_count)) return;
    attr.provides_entries.resize(provides_count);
    for (uint16_t i = 0; i < provides_count; ++i) {
        uint16_t with_count = 0;
        if (!read_u2(attr.provides_entries[i].index)) return;
        if (!read_u2(with_count)) return;
        attr.provides_entries[i].with_indices.resize(with_count);
        for (uint16_t j = 0; j < with_count; ++j) {
            if (!read_u2(attr.provides_entries[i].with_indices[j])) return;
        }
    }
}

void ClassParser::parse_module_packages_attribute(ModulePackagesAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        const uint16_t package_count = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        attr.package_index.resize(package_count);

        size_t offset = 2;
        for (uint16_t i = 0; i < package_count && offset + 2 <= data.size(); ++i) {
            attr.package_index[i] = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            offset += 2;
        }
    }
}

void ClassParser::parse_module_main_class_attribute(ModuleMainClassAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        attr.main_class_index = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    }
}

void ClassParser::parse_nest_host_attribute(NestHostAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        attr.host_class_index = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    }
}

void ClassParser::parse_nest_members_attribute(NestMembersAttribute &attr, const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        const uint16_t number_of_classes = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        attr.classes.resize(number_of_classes);

        size_t offset = 2;
        for (uint16_t i = 0; i < number_of_classes && offset + 2 <= data.size(); ++i) {
            attr.classes[i] = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            offset += 2;
        }
    }
}

void ClassParser::parse_record_attribute(RecordAttribute &attr, const std::vector<uint8_t> &data) {
    size_t offset = 0;
    auto read_u2 = [&](uint16_t &out) {
        if (offset + 2 > data.size()) return false;
        out = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
        offset += 2;
        return true;
    };
    uint16_t components_count = 0;
    if (!read_u2(components_count)) return;
    attr.components.resize(components_count);
    for (uint16_t i = 0; i < components_count; ++i) {
        if (!read_u2(attr.components[i].name_index)) return;
        if (!read_u2(attr.components[i].descriptor_index)) return;
        uint16_t attributes_count = 0;
        if (!read_u2(attributes_count)) return;
        for (uint16_t a = 0; a < attributes_count; ++a) {
            if (offset + 6 > data.size()) return;
            const uint16_t name_index = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            const uint32_t length = (static_cast<uint32_t>(data[offset + 2]) << 24) |
                                    (static_cast<uint32_t>(data[offset + 3]) << 16) |
                                    (static_cast<uint32_t>(data[offset + 4]) << 8) |
                                    (static_cast<uint32_t>(data[offset + 5]));
            offset += 6;
            if (offset + length > data.size()) return;
            CodeAttribute::AttributeInfo ai;
            ai.name = (name_index < constant_pool.size() && constant_pool[name_index] && constant_pool[name_index]->tag
                       == CONSTANT_Utf8)
                          ? constant_pool[name_index]->s_val
                          : std::string("unknown");
            ai.info.assign(data.begin() + offset, data.begin() + offset + length);
            offset += length;
            attr.components[i].attributes.push_back(std::move(ai));
        }
    }
}

void ClassParser::parse_permitted_subclasses_attribute(PermittedSubclassesAttribute &attr,
                                                       const std::vector<uint8_t> &data) {
    if (data.size() >= 2) {
        const uint16_t number_of_classes = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        attr.classes.resize(number_of_classes);

        size_t offset = 2;
        for (uint16_t i = 0; i < number_of_classes && offset + 2 <= data.size(); ++i) {
            attr.classes[i] = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
            offset += 2;
        }
    }
}

void ClassParser::dump() const {
    std::cout << "Class file: " << filename << std::endl;
    std::cout << "Version: " << major_version << "." << minor_version << std::endl;
    std::cout << "Access flags: 0x" << std::hex << access_flags << std::dec << " ("
            << access_flags_to_string(access_flags, false) << ")" << std::endl;
    std::cout << "This class: #" << this_class_index << " (" << get_class_name() << ")" << std::endl;
    std::cout << "Super class: #" << super_class_index << " (" << get_super_class_name() << ")" << std::endl;

    std::cout << "Interfaces (" << interfaces.size() << "):" << std::endl;
    for (const unsigned short interface: interfaces) {
        std::cout << "  #" << interface << " (" << get_utf8_string(interface) << ")" << std::endl;
    }

    std::cout << "Constant pool (" << constant_pool.size() << " entries):" << std::endl;
    for (uint16_t i = 1; i < constant_pool.size(); i++) {
        if (constant_pool[i]) {
            std::cout << "  #" << i << " = " << constant_pool[i]->to_string() << std::endl;
        }
    }

    std::cout << "Fields (" << fields.size() << "):" << std::endl;
    for (const auto &field: fields) {
        std::cout << "  " << field.to_string() << std::endl;
    }

    std::cout << "Methods (" << methods.size() << "):" << std::endl;
    for (const auto &method: methods) {
        std::cout << "  " << method.to_string() << std::endl;
    }

    std::cout << "Attributes (" << class_attributes.size() << "):" << std::endl;
    for (const auto &attr: class_attributes) {
        std::cout << "  " << attr.to_string() << std::endl;
    }
}
