#include "jsonvalue.h"

JsonValue::JsonValue() :
    m_value(nullptr)
{
}

JsonValue::JsonValue(JsonValue &&other)
{
    m_value = other.m_value;
    other.m_value = nullptr;
}

JsonValue &JsonValue::operator=(JsonValue &&other)
{
    if (m_value)
        delete m_value;
    m_value = other.m_value;
    other.m_value = nullptr;
    return *this;
}

JsonValue::JsonValue(bool value)
{
    m_value = new JsonBoolean(value);
}

JsonValue::JsonValue(int value)
{
    m_value = new JsonIntNumber(value);
}

JsonValue::JsonValue(long value)
{
    m_value = new JsonIntNumber((int)value);
}

JsonValue::JsonValue(uint32_t value)
{
    m_value = new JsonIntNumber((int)value);
}

JsonValue::JsonValue(float value)
{
    m_value = new JsonFloatNumber(value);
}

JsonValue::JsonValue(double value)
{
    m_value = new JsonFloatNumber((float)value);
}

JsonValue::JsonValue(const ByteArray &value)
{
    m_value = new JsonString(value);
}

JsonValue::JsonValue(const char *value)
{
    m_value = new JsonString(value);
}

JsonValue::JsonValue(const JsonArray &value)
{
    m_value = new JsonArray(value);
}

JsonValue::JsonValue(const JsonObject &value)
{
    m_value = new JsonObject(value);
}

JsonValue::~JsonValue()
{
    if (m_value)
    {
        delete m_value;
        m_value = nullptr;
    }
}

ByteArray JsonValue::toJson() const
{
    return m_value? m_value->toJson(): "null";
}

void JsonValue::toJson(ByteArray &ba) const
{
    if (m_value)
        m_value->toJson(ba);
    else
        ba.append("null");
}
