#ifndef _JSONVALUE_H
#define _JSONVALUE_H

#include "core/bytearray.h"

#include <stdint.h>
#include <vector>
#include <map>

class JsonValueBase;
class JsonArray;
class JsonObject;

class JsonValue
{
public:
    JsonValue();
    JsonValue(const JsonValue &other) = delete;
    JsonValue(JsonValue &&other);
    JsonValue &operator=(const JsonValue &other) = delete;
    JsonValue &operator=(JsonValue &&other);
    ~JsonValue();
    JsonValue(bool value);
    JsonValue(int value);
    JsonValue(long value);
    JsonValue(uint32_t value);
    JsonValue(float value);
    JsonValue(double value);
    JsonValue(const ByteArray &value);
    JsonValue(const char *value);
    JsonValue(const JsonArray &value);
    JsonValue(const JsonObject &value);

    ByteArray toJson() const;
    void toJson(ByteArray &ba) const;

private:
    JsonValueBase *m_value = nullptr;
};



class JsonValueBase
{
public:
    enum Type
    {
        Null    = 0x0,
        Boolean = 0x1,
        Number  = 0x2,
        String  = 0x3,
        Array   = 0x4,
        Object  = 0x5,
        IntNumber = 0x10 | Number,
        FloatNumber = 0x20 | Number
    };

    Type type() const {return static_cast<Type>(m_type & 0x0F);}
    virtual void toJson(ByteArray &ba) const = 0;
    ByteArray toJson() const
    {
        ByteArray ba;
        toJson(ba);
        return ba;
    }
    virtual ~JsonValueBase() {}

protected:
    friend class JsonValue;
    JsonValueBase(Type type) : m_type(type) {}
    Type m_type;
};

class JsonString : public JsonValueBase
{
public:
    JsonString(const ByteArray &value) :
        JsonValueBase(String)
        {m_value = value;}
    void toJson(ByteArray &ba) const override
    {
        ba.append('\"');
        ba.append(m_value);
        ba.append('\"');
    }
private:
    ByteArray m_value;
};

class JsonIntNumber : public JsonValueBase
{
public:
    JsonIntNumber(int value) : JsonValueBase(IntNumber) {m_value = value;}
    void toJson(ByteArray &ba) const override
    {
        ba.append(ByteArray::number(m_value));
    }
private:
    int m_value;
};

class JsonFloatNumber : public JsonValueBase
{
public:
    JsonFloatNumber(float value) : JsonValueBase(FloatNumber) {m_value = value;}
    void toJson(ByteArray &ba) const override
    {
        ba.append(ByteArray::number(m_value));//, 3));
    }
private:
    float m_value;
};

class JsonBoolean : public JsonValueBase
{
public:
    JsonBoolean(bool value) : JsonValueBase(Boolean) {m_value = value;}
    void toJson(ByteArray &ba) const override
    {
        ba.append(m_value? "true": "false");
    }
private:
    bool m_value;
};

class JsonArray : public JsonValueBase
{
public:
    JsonArray() : JsonValueBase(Array) {}
    JsonArray(const JsonArray &other) : JsonValueBase(Array)
    {
        m_size = other.m_size;
        m_array = new JsonValue[m_size];
        for (int i=0; i<m_size; i++)
            m_array[i] = std::move(other.m_array[i]);
    }

    JsonArray(JsonArray &&other) : JsonValueBase(Array)
    {
        m_array = other.m_array;
        m_size = other.m_size;
        other.m_array = nullptr;
        other.m_size = 0;
    }

    JsonArray &operator=(const JsonArray &other) = delete;
    JsonArray &operator=(JsonArray &&other) = delete;
    ~JsonArray()
    {
        delete [] m_array;
    }

    void toJson(ByteArray &ba) const override
    {
        ba.append('[');
        for (int i=0; i<m_size; i++)
        {
            m_array[i].toJson(ba);
            ba.append(',');
        }
        if (m_size)
            ba[ba.size() - 1] = ']';
        else
            ba.append(']');
    }

    JsonValue &operator[](int idx)
    {
        if (idx >= m_size)
        {
            JsonValue *a = new JsonValue[m_size + 1];
            for (int i=0; i<m_size; i++)
                a[i] = std::move(m_array[i]);
            delete [] m_array;
            m_size = idx + 1;
            m_array = a;
        }
        return m_array[idx];
    }

    JsonArray &operator <<(JsonValue &&value)
    {
        operator[](m_size) = std::move(value);
        return *this;
    }

private:
    JsonValue *m_array = nullptr;
    int m_size = 0;
};

class JsonObject : public JsonValueBase
{
public:
    JsonObject() : JsonValueBase(Object) {}
    JsonObject(const JsonObject &other) : JsonValueBase(Object)
    {
        m_size = other.m_size;
        m_keys = new ByteArray[m_size];
        m_values = new JsonValue[m_size];
        for (int i=0; i<m_size; i++)
        {
            m_keys[i] = std::move(other.m_keys[i]);
            m_values[i] = std::move(other.m_values[i]);
        }
    }

    JsonObject(JsonObject &&other) : JsonValueBase(Object)
    {
        m_keys = other.m_keys;
        m_values = other.m_values;
        m_size = other.m_size;
        other.m_keys = nullptr;
        other.m_values = nullptr;
        other.m_size = 0;
    }
    JsonObject &operator=(const JsonObject &other) = delete;
    JsonObject &operator=(JsonObject &&other) = delete;
    ~JsonObject()
    {
        delete [] m_keys;
        delete [] m_values;
    }

    void toJson(ByteArray &ba) const override
    {
        ba.append('{');
        for (int i=0; i<m_size; i++)
        {
            ba.append('\"');
            ba.append(m_keys[i]);
            ba.append("\":");
            m_values[i].toJson(ba);
            ba.append(',');
        }
        if (m_size)
            ba[ba.size() - 1] = '}';
        else
            ba.append('}');
    }

    JsonValue &operator[](const ByteArray &key)
    {
        for (int i=0; i<m_size; i++)
        {
            if (m_keys[i] == key)
                return m_values[i];
        }

        ByteArray *k = new ByteArray[m_size+1];
        JsonValue *v = new JsonValue[m_size+1];
        for (int i=0; i<m_size; i++)
        {
            k[i] = std::move(m_keys[i]);
            v[i] = std::move(m_values[i]);
        }
        delete [] m_keys;
        delete [] m_values;
        k[m_size] = std::move(key);
        m_size++;
        m_keys = k;
        m_values = v;
        return m_values[m_size - 1];
    }

private:
    ByteArray *m_keys = nullptr;
    JsonValue *m_values = nullptr;
    int m_size = 0;
};




#endif