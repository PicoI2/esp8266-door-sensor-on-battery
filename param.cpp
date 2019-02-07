#include "param.h"
#include <EEPROM.h>

// Class CParamByte
void CParamByte::write(size_t& aPos)
{
    EEPROM.write(aPos++, mValue);
}

void CParamByte::read(size_t& aPos)
{
    mValue = EEPROM.read(aPos++);
}

String CParamByte::toHtmlForm()
{
    String ret = String(mName) + ":<input type='number' name='" + String(mName) + "' value='" + String(mValue) + "'><br>";
    return ret;
}

String CParamByte::getValue()
{
    return String(mValue);
}

byte CParamByte::getByteValue()
{
    return mValue;
}

void CParamByte::setValue(String& aValue)
{
    mValue = aValue.toInt();
}

// Class CParamBool
String CParamBool::toHtmlForm()
{
    String ret = String(mName) + ":<input type='checkbox' name='" + String(mName) + "' value='" + String(mValue) + "'><br>";
    return ret;
}

String CParamBool::getValue()
{
    return mValue ? String("true") : String("false");
}

// Class CParamString
void CParamString::write(size_t& aPos)
{
    size_t size = mValue.length();
    EEPROM.write(aPos++, size);
    for (size_t i = 0; i < size; ++i) {
        EEPROM.write(aPos++, mValue.charAt(i));
    }
}

void CParamString::read(size_t& aPos)
{
    size_t size = EEPROM.read(aPos++);
    for (size_t i = 0; i < size; ++i) {
        mValue += (char)EEPROM.read(aPos++);
    }
}

String CParamString::toHtmlForm()
{
    String ret = String(mName) + ":<input type='text' name='" + String(mName) + "' value='" + mValue + "'><br>";
    return ret;
}

String CParamString::getValue()
{
    return mValue;
}

byte CParamString::getByteValue()
{
    return mValue.toInt();
}

void CParamString::setValue(String& aValue)
{
    mValue = aValue;
}