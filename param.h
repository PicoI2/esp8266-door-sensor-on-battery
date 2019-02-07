#include <ESP8266WiFi.h>

class CParam {
  public:
    CParam(const char* aName) : mName(aName) {}
    virtual void write(size_t& aPos) = 0;
    virtual void read(size_t& aPos) = 0;
    virtual String toHtmlForm() = 0;
    virtual String getValue() = 0;
    virtual byte getByteValue() = 0;
    virtual void setValue(String& aValue) = 0;

    const char* getName() { return mName; }

  protected:
    const char* mName;
};

class CParamByte : public CParam {
  public:
    CParamByte(const char* aName) : CParam(aName) {}
    virtual void write(size_t& aPos);
    virtual void read(size_t& aPos);
    virtual String toHtmlForm();
    virtual String getValue();
    virtual byte getByteValue();
    virtual void setValue(String& aValue);

  protected:
    byte mValue = 0;
};

class CParamBool : public CParamByte {
  public:
    CParamBool(const char* aName) : CParamByte(aName) {}
    virtual String toHtmlForm();
    virtual String getValue();
};

class CParamString : public CParam {
  public:
    CParamString(const char* aName) : CParam(aName) {}
    virtual void write(size_t& aPos);
    virtual void read(size_t& aPos);
    virtual String toHtmlForm();
    virtual String getValue();
    virtual byte getByteValue();
    virtual void setValue(String& aValue);

  protected:
    String mValue;
};