#pragma once

#include <string>
#include <vector>
#include <memory>

class VirtualMachine;

class ClassParser {
public:
    enum {
        CONSTANT_Utf8 = 1,
        CONSTANT_Integer = 3,
        CONSTANT_Float = 4,
        CONSTANT_Long = 5,
        CONSTANT_Double = 6,
        CONSTANT_Class = 7,
        CONSTANT_String = 8,
        CONSTANT_Fieldref = 9,
        CONSTANT_Methodref = 10,
        CONSTANT_InterfaceMethodref = 11,
        CONSTANT_NameAndType = 12,
        CONSTANT_MethodHandle = 15,
        CONSTANT_MethodType = 16,
        CONSTANT_Dynamic = 17,
        CONSTANT_InvokeDynamic = 18,
        CONSTANT_Module = 19,
        CONSTANT_Package = 20,
    };

    struct ConstantPoolInfo {
        uint8_t tag;
        std::string s_val;
        uint32_t i_val = 0;
        uint64_t l_val = 0;
        uint16_t index1 = 0;
        uint16_t index2 = 0;
        uint8_t reference_kind = 0;

        std::string to_string() const;
    };

    struct ExceptionTableEntry {
        uint16_t start_pc;
        uint16_t end_pc;
        uint16_t handler_pc;
        uint16_t catch_type;

        std::string to_string() const;
    };

    struct CodeAttribute {
        uint16_t max_stack;
        uint16_t max_locals;
        std::vector<uint8_t> code;
        std::vector<ExceptionTableEntry> exception_table;
        struct AttributeInfo {
            std::string name;
            std::vector<uint8_t> info;

            std::string to_string() const;
        };
        std::vector<AttributeInfo> attributes;

        std::string to_string() const;
    };

    struct SourceFileAttribute {
        uint16_t sourcefile_index;

        std::string to_string() const;
    };

    struct LineNumberTableAttribute {
        struct LineNumberEntry {
            uint16_t start_pc;
            uint16_t line_number;

            std::string to_string() const;
        };
        std::vector<LineNumberEntry> line_number_table;

        std::string to_string() const;
    };

    struct LocalVariableTableAttribute {
        struct LocalVariableEntry {
            uint16_t start_pc;
            uint16_t length;
            uint16_t name_index;
            uint16_t descriptor_index;
            uint16_t index;

            std::string to_string() const;
        };
        std::vector<LocalVariableEntry> local_variable_table;

        std::string to_string() const;
    };

    struct ExceptionsAttribute {
        std::vector<uint16_t> exception_index_table;

        std::string to_string() const;
    };

    struct ConstantValueAttribute {
        uint16_t constantvalue_index;

        std::string to_string() const;
    };

    struct BootstrapMethodsAttribute {
        struct BootstrapMethod {
            uint16_t bootstrap_method_ref;
            std::vector<uint16_t> bootstrap_arguments;

            std::string to_string() const;
        };
        std::vector<BootstrapMethod> bootstrap_methods;

        std::string to_string() const;
    };

    struct SignatureAttribute {
        uint16_t signature_index;

        std::string to_string() const;
    };

    struct DeprecatedAttribute {
        std::string to_string() const;
    };

    struct SyntheticAttribute {
        std::string to_string() const;
    };

    struct InnerClassesAttribute {
        struct InnerClass {
            uint16_t inner_class_info_index;
            uint16_t outer_class_info_index;
            uint16_t inner_name_index;
            uint16_t inner_class_access_flags;

            std::string to_string() const;
        };
        std::vector<InnerClass> classes;

        std::string to_string() const;
    };

    struct EnclosingMethodAttribute {
        uint16_t class_index;
        uint16_t method_index;

        std::string to_string() const;
    };

    struct SourceDebugExtensionAttribute {
        std::vector<uint8_t> debug_extension;

        std::string to_string() const;
    };

    struct LocalVariableTypeTableAttribute {
        struct LocalVariableTypeEntry {
            uint16_t start_pc;
            uint16_t length;
            uint16_t name_index;
            uint16_t signature_index;
            uint16_t index;

            std::string to_string() const;
        };
        std::vector<LocalVariableTypeEntry> local_variable_type_table;

        std::string to_string() const;
    };

    struct MethodParametersAttribute {
        struct Parameter {
            uint16_t name_index;
            uint16_t access_flags;

            std::string to_string() const;
        };
        std::vector<Parameter> parameters;

        std::string to_string() const;
    };

    struct RuntimeVisibleAnnotationsAttribute {
        uint16_t num_annotations = 0;
        std::vector<uint8_t> annotations;

        std::string to_string() const;
    };

    struct RuntimeInvisibleAnnotationsAttribute {
        uint16_t num_annotations = 0;
        std::vector<uint8_t> annotations;

        std::string to_string() const;
    };

    struct RuntimeVisibleParameterAnnotationsAttribute {
        uint8_t num_parameters = 0;
        std::vector<std::vector<uint8_t>> parameter_annotations;

        std::string to_string() const;
    };

    struct RuntimeInvisibleParameterAnnotationsAttribute {
        uint8_t num_parameters = 0;
        std::vector<std::vector<uint8_t>> parameter_annotations;

        std::string to_string() const;
    };

    struct RuntimeVisibleTypeAnnotationsAttribute {
        uint16_t num_annotations = 0;
        std::vector<uint8_t> type_annotations;

        std::string to_string() const;
    };

    struct RuntimeInvisibleTypeAnnotationsAttribute {
        uint16_t num_annotations = 0;
        std::vector<uint8_t> type_annotations;

        std::string to_string() const;
    };

    struct AnnotationDefaultAttribute {
        std::vector<uint8_t> default_value;

        std::string to_string() const;
    };

    struct ModuleAttribute {
        uint16_t module_name_index;
        uint16_t module_flags;
        uint16_t module_version_index;
        struct RequiresEntry {
            uint16_t index;
            uint16_t flags;
            uint16_t version_index;
            std::string to_string() const;
        };
        struct ExportsEntry {
            uint16_t index;
            uint16_t flags;
            std::vector<uint16_t> to_indices;
            std::string to_string() const;
        };
        struct OpensEntry {
            uint16_t index;
            uint16_t flags;
            std::vector<uint16_t> to_indices;
            std::string to_string() const;
        };
        std::vector<RequiresEntry> requires_entries;
        std::vector<ExportsEntry> exports_entries;
        std::vector<OpensEntry> opens_entries;
        std::vector<uint16_t> uses;
        struct ProvidesEntry {
            uint16_t index;
            std::vector<uint16_t> with_indices;
            std::string to_string() const;
        };
        std::vector<ProvidesEntry> provides_entries;

        std::string to_string() const;
    };

    struct ModulePackagesAttribute {
        std::vector<uint16_t> package_index;

        std::string to_string() const;
    };

    struct ModuleMainClassAttribute {
        uint16_t main_class_index;

        std::string to_string() const;
    };

    struct NestHostAttribute {
        uint16_t host_class_index;

        std::string to_string() const;
    };

    struct NestMembersAttribute {
        std::vector<uint16_t> classes;

        std::string to_string() const;
    };

    struct RecordAttribute {
        struct ComponentInfo {
            uint16_t name_index;
            uint16_t descriptor_index;
            std::vector<CodeAttribute::AttributeInfo> attributes;

            std::string to_string() const;
        };
        std::vector<ComponentInfo> components;

        std::string to_string() const;
    };

    struct PermittedSubclassesAttribute {
        std::vector<uint16_t> classes;

        std::string to_string() const;
    };

    struct SpecializedAttribute {
        enum Type {
            UNKNOWN,
            SOURCE_FILE,
            LINE_NUMBER_TABLE,
            LOCAL_VARIABLE_TABLE,
            EXCEPTIONS,
            CONSTANT_VALUE,
            BOOTSTRAP_METHODS,
            SIGNATURE,
            DEPRECATED,
            SYNTHETIC,
            INNER_CLASSES,
            ENCLOSING_METHOD,
            SOURCE_DEBUG_EXTENSION,
            LOCAL_VARIABLE_TYPE_TABLE,
            METHOD_PARAMETERS,
            RUNTIME_VISIBLE_ANNOTATIONS,
            RUNTIME_INVISIBLE_ANNOTATIONS,
            RUNTIME_VISIBLE_PARAMETER_ANNOTATIONS,
            RUNTIME_INVISIBLE_PARAMETER_ANNOTATIONS,
            RUNTIME_VISIBLE_TYPE_ANNOTATIONS,
            RUNTIME_INVISIBLE_TYPE_ANNOTATIONS,
            ANNOTATION_DEFAULT,
            MODULE,
            MODULE_PACKAGES,
            MODULE_MAIN_CLASS,
            NEST_HOST,
            NEST_MEMBERS,
            RECORD,
            PERMITTED_SUBCLASSES
        };
        Type type = UNKNOWN;
        std::string name;
        std::vector<uint8_t> raw_data;

        SourceFileAttribute source_file;
        LineNumberTableAttribute line_number_table;
        LocalVariableTableAttribute local_variable_table;
        ExceptionsAttribute exceptions;
        ConstantValueAttribute constant_value;
        BootstrapMethodsAttribute bootstrap_methods;
        SignatureAttribute signature;
        DeprecatedAttribute deprecated;
        SyntheticAttribute synthetic;
        InnerClassesAttribute inner_classes;
        EnclosingMethodAttribute enclosing_method;
        SourceDebugExtensionAttribute source_debug_extension;
        LocalVariableTypeTableAttribute local_variable_type_table;
        MethodParametersAttribute method_parameters;
        RuntimeVisibleAnnotationsAttribute runtime_visible_annotations;
        RuntimeInvisibleAnnotationsAttribute runtime_invisible_annotations;
        RuntimeVisibleParameterAnnotationsAttribute runtime_visible_parameter_annotations;
        RuntimeInvisibleParameterAnnotationsAttribute runtime_invisible_parameter_annotations;
        RuntimeVisibleTypeAnnotationsAttribute runtime_visible_type_annotations;
        RuntimeInvisibleTypeAnnotationsAttribute runtime_invisible_type_annotations;
        AnnotationDefaultAttribute annotation_default;
        ModuleAttribute module_attr;
        ModulePackagesAttribute module_packages;
        ModuleMainClassAttribute module_main_class;
        NestHostAttribute nest_host;
        NestMembersAttribute nest_members;
        RecordAttribute record;
        PermittedSubclassesAttribute permitted_subclasses;

        std::string to_string() const;
    };

    struct MethodInfo {
        uint16_t access_flags;
        uint16_t name_index;
        uint16_t descriptor_index;
        uint16_t attributes_count;
        CodeAttribute* code_attribute = nullptr;
        std::string name;
        std::string descriptor;
        std::vector<CodeAttribute::AttributeInfo> attributes;

        std::string to_string() const;
        ~MethodInfo();
    };

    struct FieldInfo {
        uint16_t access_flags;
        uint16_t name_index;
        uint16_t descriptor_index;
        uint16_t attributes_count;
        std::string name;
        std::string descriptor;
        std::vector<CodeAttribute::AttributeInfo> attributes;

        std::string to_string() const;
    };
    // Constants for access flags
    static constexpr uint16_t ACC_PUBLIC = 0x0001;
    static constexpr uint16_t ACC_PRIVATE = 0x0002;
    static constexpr uint16_t ACC_PROTECTED = 0x0004;
    static constexpr uint16_t ACC_STATIC = 0x0008;
    static constexpr uint16_t ACC_FINAL = 0x0010;
    static constexpr uint16_t ACC_SYNCHRONIZED = 0x0020;
    static constexpr uint16_t ACC_VOLATILE = 0x0040;
    static constexpr uint16_t ACC_TRANSIENT = 0x0080;
    static constexpr uint16_t ACC_NATIVE = 0x0100;
    static constexpr uint16_t ACC_ABSTRACT = 0x0400;
    static constexpr uint16_t ACC_STRICT = 0x0800;
    static constexpr uint16_t ACC_SYNTHETIC = 0x1000;
    static constexpr uint16_t ACC_BRIDGE = 0x0040;
    static constexpr uint16_t ACC_VARARGS = 0x0080;
    static constexpr uint16_t ACC_ENUM = 0x4000;
    static constexpr uint16_t ACC_MANDATED = 0x8000;

    ClassParser(const std::string &filename);
    ~ClassParser();

    void parse();
    void dump() const;

    MethodInfo *find_main_method();
    MethodInfo *find_method(const std::string &name);
    MethodInfo *find_method(const std::string &name, const std::string &descriptor);

    const std::vector<ConstantPoolInfo *> &get_constant_pool() const;
    const std::vector<FieldInfo> &get_fields() const { return fields; }
    const std::vector<MethodInfo> &get_methods() const { return methods; }
    const std::string &get_class_name() const;
    const std::string &get_super_class_name() const;
    uint16_t get_major_version() const { return major_version; }
    uint16_t get_minor_version() const { return minor_version; }
    uint16_t get_access_flags() const { return access_flags; }

    std::string get_utf8_string(uint16_t index) const;
    std::string get_class_name(uint16_t index) const;
    std::string get_super_class_name(uint16_t index) const;
    std::string get_method_descriptor(uint16_t index) const;
    std::string get_field_descriptor(uint16_t index) const;
    const std::vector<CodeAttribute::AttributeInfo>& get_class_attributes() const { return class_attributes; }

    std::string get_method_name(const MethodInfo& method) const;
    std::string get_field_name(const FieldInfo& field) const;
    std::string get_access_flags_string(uint16_t flags, bool is_method = false) const;
    std::string to_string() const;

    typedef std::vector<FieldInfo>::const_iterator field_iterator;
    typedef std::vector<MethodInfo>::const_iterator method_iterator;

    field_iterator fields_begin() const { return fields.begin(); }
    field_iterator fields_end() const { return fields.end(); }
    method_iterator methods_begin() const { return methods.begin(); }
    method_iterator methods_end() const { return methods.end(); }

private:
    std::string filename;
    uint8_t *file_data;
    size_t file_size = 0;
    size_t cursor = 0;
    std::string class_name;
    std::string super_class_name;

    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t access_flags;
    uint16_t this_class_index;
    uint16_t super_class_index;
    uint16_t interfaces_count;

    std::vector<ConstantPoolInfo *> constant_pool;
    std::vector<MethodInfo> methods;
    std::vector<FieldInfo> fields;
    std::vector<uint16_t> interfaces;
    std::vector<CodeAttribute::AttributeInfo> class_attributes;

    bool load_file();
    void ensure_available(size_t bytes) const;
    uint8_t read_uint8();
    uint16_t read_uint16();
    uint32_t read_uint32();
    uint64_t read_uint64();

    std::string read_modified_utf8(uint16_t length);

    uint8_t read_u1();

    void parse_constant_pool();

    void parse_access_flags();

    void parse_this_class();

    void parse_super_class();

    void parse_interfaces();
    void parse_fields();
    void parse_methods();
    void parse_attributes(uint16_t count, MethodInfo* method = nullptr,
                         FieldInfo* field = nullptr,
                         std::vector<CodeAttribute::AttributeInfo>* out_attrs = nullptr);
    void parse_code_attribute(CodeAttribute* code_attr);

    SpecializedAttribute parse_specialized_attribute(const std::string& name, const std::vector<uint8_t>& data);
    void parse_source_file_attribute(SourceFileAttribute& attr, const std::vector<uint8_t>& data);
    void parse_line_number_table_attribute(LineNumberTableAttribute& attr, const std::vector<uint8_t>& data);
    void parse_local_variable_table_attribute(LocalVariableTableAttribute& attr, const std::vector<uint8_t>& data);
    void parse_exceptions_attribute(ExceptionsAttribute& attr, const std::vector<uint8_t>& data);
    void parse_constant_value_attribute(ConstantValueAttribute& attr, const std::vector<uint8_t>& data);
    void parse_bootstrap_methods_attribute(BootstrapMethodsAttribute& attr, const std::vector<uint8_t>& data);
    void parse_signature_attribute(SignatureAttribute& attr, const std::vector<uint8_t>& data);
    void parse_deprecated_attribute(DeprecatedAttribute& attr, const std::vector<uint8_t>& data);
    void parse_synthetic_attribute(SyntheticAttribute& attr, const std::vector<uint8_t>& data);
    void parse_inner_classes_attribute(InnerClassesAttribute& attr, const std::vector<uint8_t>& data);
    void parse_enclosing_method_attribute(EnclosingMethodAttribute& attr, const std::vector<uint8_t>& data);
    void parse_source_debug_extension_attribute(SourceDebugExtensionAttribute& attr, const std::vector<uint8_t>& data);
    void parse_local_variable_type_table_attribute(LocalVariableTypeTableAttribute& attr, const std::vector<uint8_t>& data);
    void parse_method_parameters_attribute(MethodParametersAttribute& attr, const std::vector<uint8_t>& data);
    void parse_runtime_visible_annotations_attribute(RuntimeVisibleAnnotationsAttribute& attr, const std::vector<uint8_t>& data);
    void parse_runtime_invisible_annotations_attribute(RuntimeInvisibleAnnotationsAttribute& attr, const std::vector<uint8_t>& data);
    void parse_runtime_visible_parameter_annotations_attribute(RuntimeVisibleParameterAnnotationsAttribute& attr, const std::vector<uint8_t>& data);
    void parse_runtime_invisible_parameter_annotations_attribute(RuntimeInvisibleParameterAnnotationsAttribute& attr, const std::vector<uint8_t>& data);
    void parse_runtime_visible_type_annotations_attribute(RuntimeVisibleTypeAnnotationsAttribute& attr, const std::vector<uint8_t>& data);
    void parse_runtime_invisible_type_annotations_attribute(RuntimeInvisibleTypeAnnotationsAttribute& attr, const std::vector<uint8_t>& data);
    void parse_annotation_default_attribute(AnnotationDefaultAttribute& attr, const std::vector<uint8_t>& data);
    void parse_module_attribute(ModuleAttribute& attr, const std::vector<uint8_t>& data);
    void parse_module_packages_attribute(ModulePackagesAttribute& attr, const std::vector<uint8_t>& data);
    void parse_module_main_class_attribute(ModuleMainClassAttribute& attr, const std::vector<uint8_t>& data);
    void parse_nest_host_attribute(NestHostAttribute& attr, const std::vector<uint8_t>& data);
    void parse_nest_members_attribute(NestMembersAttribute& attr, const std::vector<uint8_t>& data);
    void parse_record_attribute(RecordAttribute& attr, const std::vector<uint8_t>& data);
    void parse_permitted_subclasses_attribute(PermittedSubclassesAttribute& attr, const std::vector<uint8_t>& data);
};
