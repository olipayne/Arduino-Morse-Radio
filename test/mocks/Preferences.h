#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <map>
#include <string>

#include "Arduino.h"

class Preferences {
 public:
  bool begin(const char* name, bool readOnly = false) {
    currentNamespace = (name == nullptr) ? "" : name;
    isReadOnly = readOnly;
    return true;
  }

  void end() {}

  bool clear() {
    auto prefix = currentNamespace + ":";
    eraseByPrefix(intStore(), prefix);
    eraseByPrefix(uintStore(), prefix);
    eraseByPrefix(ucharStore(), prefix);
    eraseByPrefix(boolStore(), prefix);
    eraseByPrefix(stringStore(), prefix);
    return true;
  }

  size_t putInt(const char* key, int value) {
    if (isReadOnly) return 0;
    intStore()[fullKey(key)] = value;
    return sizeof(int);
  }

  size_t putUInt(const char* key, unsigned int value) {
    if (isReadOnly) return 0;
    uintStore()[fullKey(key)] = value;
    return sizeof(unsigned int);
  }

  size_t putUChar(const char* key, unsigned char value) {
    if (isReadOnly) return 0;
    ucharStore()[fullKey(key)] = value;
    return sizeof(unsigned char);
  }

  size_t putBool(const char* key, bool value) {
    if (isReadOnly) return 0;
    boolStore()[fullKey(key)] = value;
    return sizeof(bool);
  }

  size_t putString(const char* key, const String& value) {
    if (isReadOnly) return 0;
    stringStore()[fullKey(key)] = value;
    return value.length();
  }

  int getInt(const char* key, int defaultValue = 0) const {
    auto it = intStore().find(fullKey(key));
    return (it == intStore().end()) ? defaultValue : it->second;
  }

  unsigned int getUInt(const char* key, unsigned int defaultValue = 0) const {
    auto it = uintStore().find(fullKey(key));
    return (it == uintStore().end()) ? defaultValue : it->second;
  }

  unsigned char getUChar(const char* key, unsigned char defaultValue = 0) const {
    auto it = ucharStore().find(fullKey(key));
    return (it == ucharStore().end()) ? defaultValue : it->second;
  }

  bool getBool(const char* key, bool defaultValue = false) const {
    auto it = boolStore().find(fullKey(key));
    return (it == boolStore().end()) ? defaultValue : it->second;
  }

  String getString(const char* key, const String& defaultValue = "") const {
    auto it = stringStore().find(fullKey(key));
    return (it == stringStore().end()) ? defaultValue : it->second;
  }

 private:
  template <typename MapType>
  static void eraseByPrefix(MapType& store, const std::string& prefix) {
    for (auto it = store.begin(); it != store.end();) {
      if (it->first.compare(0, prefix.size(), prefix) == 0) {
        it = store.erase(it);
      } else {
        ++it;
      }
    }
  }

  std::string fullKey(const char* key) const {
    return currentNamespace + ":" + (key == nullptr ? "" : key);
  }

  static std::map<std::string, int>& intStore() {
    static std::map<std::string, int> store;
    return store;
  }

  static std::map<std::string, unsigned int>& uintStore() {
    static std::map<std::string, unsigned int> store;
    return store;
  }

  static std::map<std::string, unsigned char>& ucharStore() {
    static std::map<std::string, unsigned char> store;
    return store;
  }

  static std::map<std::string, bool>& boolStore() {
    static std::map<std::string, bool> store;
    return store;
  }

  static std::map<std::string, String>& stringStore() {
    static std::map<std::string, String> store;
    return store;
  }

  std::string currentNamespace;
  bool isReadOnly = false;
};

#endif
