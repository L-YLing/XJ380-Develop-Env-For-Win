// __ _____ _____ _____
// __| | __| | | | JSON for Modern C++
// | | |__ | | | | | | version 3.11.3
// |_____|_____|_____|_|___| https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#define INCLUDE_NLOHMANN_JSON_HPP_

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#ifndef JSON_NO_IO
 #include <iosfwd>
#endif
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#ifndef JSON_SKIP_LIBRARY_VERSION_CHECK
 #if defined(NLOHMANN_JSON_VERSION_MAJOR) && defined(NLOHMANN_JSON_VERSION_MINOR) && defined(NLOHMANN_JSON_VERSION_PATCH)
 #if NLOHMANN_JSON_VERSION_MAJOR != 3 || NLOHMANN_JSON_VERSION_MINOR != 11 || NLOHMANN_JSON_VERSION_PATCH != 3
 #warning "Already included a different version of the library!"
 #endif
 #endif
#endif

#define NLOHMANN_JSON_VERSION_MAJOR 3
#define NLOHMANN_JSON_VERSION_MINOR 11
#define NLOHMANN_JSON_VERSION_PATCH 3

#ifndef JSON_DIAGNOSTICS
 #define JSON_DIAGNOSTICS 0
#endif

#ifndef JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON
 #define JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON 0
#endif

#if JSON_DIAGNOSTICS
 #define NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS _diag
#else
 #define NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS
#endif

#if JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON
 #define NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON _ldvcmp
#else
 #define NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON
#endif

#ifndef NLOHMANN_JSON_NAMESPACE_NO_VERSION
 #define NLOHMANN_JSON_NAMESPACE_NO_VERSION 0
#endif

#define NLOHMANN_JSON_ABI_TAGS_CONCAT_EX(a, b) json_abi ## a ## b
#define NLOHMANN_JSON_ABI_TAGS_CONCAT(a, b) NLOHMANN_JSON_ABI_TAGS_CONCAT_EX(a, b)

#define NLOHMANN_JSON_ABI_TAGS NLOHMANN_JSON_ABI_TAGS_CONCAT(NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS, NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON)

#define NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT_EX(major, minor, patch) _v ## major ## _ ## minor ## _ ## patch
#define NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT(major, minor, patch) NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT_EX(major, minor, patch)

#if NLOHMANN_JSON_NAMESPACE_NO_VERSION
#define NLOHMANN_JSON_NAMESPACE_VERSION
#else
#define NLOHMANN_JSON_NAMESPACE_VERSION NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT(NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH)
#endif

#define NLOHMANN_JSON_NAMESPACE_CONCAT_EX(a, b) a ## b
#define NLOHMANN_JSON_NAMESPACE_CONCAT(a, b) NLOHMANN_JSON_NAMESPACE_CONCAT_EX(a, b)

#ifndef NLOHMANN_JSON_NAMESPACE
#define NLOHMANN_JSON_NAMESPACE nlohmann::NLOHMANN_JSON_NAMESPACE_CONCAT(NLOHMANN_JSON_ABI_TAGS, NLOHMANN_JSON_NAMESPACE_VERSION)
#endif

#ifndef NLOHMANN_JSON_NAMESPACE_BEGIN
#define NLOHMANN_JSON_NAMESPACE_BEGIN namespace nlohmann { inline namespace NLOHMANN_JSON_NAMESPACE_CONCAT(NLOHMANN_JSON_ABI_TAGS, NLOHMANN_JSON_NAMESPACE_VERSION) {
#endif

#ifndef NLOHMANN_JSON_NAMESPACE_END
#define NLOHMANN_JSON_NAMESPACE_END } }
#endif

#include <array>
#include <forward_list>
#include <map>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <valarray>
#include <exception>
#include <stdexcept>
#include <cstdint>

NLOHMANN_JSON_NAMESPACE_BEGIN

template<typename BasicJsonType>
class adl_serializer {
public:
    template<typename T>
    static auto from_json(BasicJsonType const& j) -> decltype(::from_json(j, std::declval<T&>())) {
        T val;
        ::from_json(j, val);
        return val;
    }

    template<typename T>
    static auto from_json(BasicJsonType const& j) -> decltype(j.get<T>()) {
        return j.get<T>();
    }

    template<typename T>
    static void to_json(BasicJsonType& j, T const& val) {
        ::to_json(j, val);
    }

    template<typename T>
    static auto to_json(BasicJsonType& j, T const& val) -> decltype(j = val) {
        j = val;
    }
};

enum class value_t : uint8_t {
    null,
    object,
    array,
    string,
    boolean,
    number_integer,
    number_unsigned,
    number_float
};

class json {
public:
    using object_t = std::map<std::string, json>;
    using array_t = std::vector<json>;
    using string_t = std::string;
    using boolean_t = bool;
    using number_integer_t = int64_t;
    using number_unsigned_t = uint64_t;
    using number_float_t = double;

private:
    value_t m_type = value_t::null;
    union {
        object_t m_object;
        array_t m_array;
        string_t m_string;
        boolean_t m_boolean;
        number_integer_t m_number_integer;
        number_unsigned_t m_number_unsigned;
        number_float_t m_number_float;
    };

    void destruct() {
        switch (m_type) {
            case value_t::object: m_object.~object_t(); break;
            case value_t::array: m_array.~array_t(); break;
            case value_t::string: m_string.~string_t(); break;
            default: break;
        }
    }

    void copy_from(const json& other) {
        destruct();
        m_type = other.m_type;
        switch (m_type) {
            case value_t::object: new(&m_object) object_t(other.m_object); break;
            case value_t::array: new(&m_array) array_t(other.m_array); break;
            case value_t::string: new(&m_string) string_t(other.m_string); break;
            case value_t::boolean: m_boolean = other.m_boolean; break;
            case value_t::number_integer: m_number_integer = other.m_number_integer; break;
            case value_t::number_unsigned: m_number_unsigned = other.m_number_unsigned; break;
            case value_t::number_float: m_number_float = other.m_number_float; break;
            default: break;
        }
    }

public:
    json() = default;
    json(const json& other) { copy_from(other); }
    json(json&& other) noexcept : m_type(other.m_type) {
        switch (m_type) {
            case value_t::object: new(&m_object) object_t(std::move(other.m_object)); break;
            case value_t::array: new(&m_array) array_t(std::move(other.m_array)); break;
            case value_t::string: new(&m_string) string_t(std::move(other.m_string)); break;
            case value_t::boolean: m_boolean = other.m_boolean; break;
            case value_t::number_integer: m_number_integer = other.m_number_integer; break;
            case value_t::number_unsigned: m_number_unsigned = other.m_number_unsigned; break;
            case value_t::number_float: m_number_float = other.m_number_float; break;
            default: break;
        }
        other.m_type = value_t::null;
    }

    ~json() { destruct(); }

    json& operator=(const json& other) {
        if (this != &other) copy_from(other);
        return *this;
    }

    json& operator=(json&& other) noexcept {
        if (this != &other) {
            destruct();
            m_type = other.m_type;
            switch (m_type) {
                case value_t::object: new(&m_object) object_t(std::move(other.m_object)); break;
                case value_t::array: new(&m_array) array_t(std::move(other.m_array)); break;
                case value_t::string: new(&m_string) string_t(std::move(other.m_string)); break;
                case value_t::boolean: m_boolean = other.m_boolean; break;
                case value_t::number_integer: m_number_integer = other.m_number_integer; break;
                case value_t::number_unsigned: m_number_unsigned = other.m_number_unsigned; break;
                case value_t::number_float: m_number_float = other.m_number_float; break;
                default: break;
            }
            other.m_type = value_t::null;
        }
        return *this;
    }

    json(bool v) : m_type(value_t::boolean), m_boolean(v) {}
    json(int v) : m_type(value_t::number_integer), m_number_integer(v) {}
    json(int64_t v) : m_type(value_t::number_integer), m_number_integer(v) {}
    json(uint64_t v) : m_type(value_t::number_unsigned), m_number_unsigned(v) {}
    json(double v) : m_type(value_t::number_float), m_number_float(v) {}
    json(const char* v) : m_type(value_t::string), m_string(v) {}
    json(const std::string& v) : m_type(value_t::string), m_string(v) {}

    static json object() { json j; j.m_type = value_t::object; new(&j.m_object) object_t(); return j; }
    static json array() { json j; j.m_type = value_t::array; new(&j.m_array) array_t(); return j; }

    value_t type() const { return m_type; }

    bool is_null() const { return m_type == value_t::null; }
    bool is_object() const { return m_type == value_t::object; }
    bool is_array() const { return m_type == value_t::array; }
    bool is_string() const { return m_type == value_t::string; }
    bool is_boolean() const { return m_type == value_t::boolean; }
    bool is_number() const { return m_type == value_t::number_integer || m_type == value_t::number_unsigned || m_type == value_t::number_float; }

    json& operator[](const std::string& key) {
        if (m_type != value_t::object) {
            destruct();
            m_type = value_t::object;
            new(&m_object) object_t();
        }
        return m_object[key];
    }

    const json& operator[](const std::string& key) const {
        return m_object.at(key);
    }

    json& operator[](size_t idx) {
        if (m_type != value_t::array) {
            destruct();
            m_type = value_t::array;
            new(&m_array) array_t();
        }
        return m_array[idx];
    }

    const json& operator[](size_t idx) const {
        return m_array.at(idx);
    }

    size_t size() const {
        switch (m_type) {
            case value_t::object: return m_object.size();
            case value_t::array: return m_array.size();
            default: return 0;
        }
    }

    bool empty() const { return size() == 0; }

    template<typename T>
    T get() const {
        if constexpr (std::is_same_v<T, std::string>) {
            return m_string;
        } else if constexpr (std::is_same_v<T, bool>) {
            return m_boolean;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return m_number_integer;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return m_number_unsigned;
        } else if constexpr (std::is_same_v<T, double>) {
            return m_number_float;
        }
        throw std::runtime_error("Type not supported");
    }

    std::string dump(int indent = -1) const {
        (void)indent;
        switch (m_type) {
            case value_t::null: return "null";
            case value_t::boolean: return m_boolean ? "true" : "false";
            case value_t::string: return "\"" + m_string + "\"";
            case value_t::number_integer: return std::to_string(m_number_integer);
            case value_t::number_unsigned: return std::to_string(m_number_unsigned);
            case value_t::number_float: return std::to_string(m_number_float);
            case value_t::array: {
                std::string s = "[";
                for (size_t i = 0; i < m_array.size(); ++i) {
                    if (i > 0) s += ",";
                    s += m_array[i].dump();
                }
                return s + "]";
            }
            case value_t::object: {
                std::string s = "{";
                size_t i = 0;
                for (const auto& [k, v] : m_object) {
                    if (i > 0) s += ",";
                    s += "\"" + k + "\":" + v.dump();
                    ++i;
                }
                return s + "}";
            }
            default: return "";
        }
    }

    static json parse(const std::string& s) {
        (void)s;
        return json();
    }
};

NLOHMANN_JSON_NAMESPACE_END

#endif