#pragma once
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include "ecs.h"
#include "registry.h"
#include "parser.h"

struct IField {
    virtual ~IField() = default;
    virtual void read(void* obj, std::string_view& sv) = 0;
};

template<typename T, typename Member>
struct Field : IField {
    Member T::* memberPtr;

    Field(Member T::* ptr) : memberPtr(ptr) {}

    void read(void* obj, std::string_view& sv) override {
        T* typedObj = static_cast<T*>(obj);
        Parser::parse<Member>(sv, typedObj->*memberPtr);
    }
};

template<typename T, typename Member>
IField* makeField(Member T::* memberPtr) {
    return new Field<T, Member>(memberPtr);
}

template<typename T>
using Schema = std::vector<std::unique_ptr<IField>>;

class SchemaRegistry {
public:
    static SchemaRegistry& instance() {
        static SchemaRegistry registry;
        return registry;
    }

    template<typename T>
    Schema<T>& get() {
        std::type_index typeIdx(typeid(T));
        return m_schemas[typeIdx];
    }

private:
    std::unordered_map<std::type_index, std::vector<std::unique_ptr<IField>>> m_schemas;
};

template<typename T>
struct AutoRegisterSchema {
    AutoRegisterSchema(std::initializer_list<IField*> fields) {
        auto& schema = SchemaRegistry::instance().get<T>();

        for (auto* field : fields) {
            schema.emplace_back(field);
        }
    }
};
