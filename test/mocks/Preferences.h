#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <map>
#include <string>

class Preferences {
 public:
  bool begin(const char* name, bool readOnly = false) { return true; }

  void end() {
    // Do nothing
  }

  bool clear() {
    intValues.clear();
    stringValues.clear();
    return true;
  }

  size_t putInt(const char* key, int value) {
    intValues[key] = value;
    return sizeof(int);
  }

  size_t putString(const char* key, const std::string& value) {
    stringValues[key] = value;
    return value.length();
  }

  int getInt(const char* key, int defaultValue = 0) {
    auto it = intValues.find(key);
    if (it != intValues.end()) {
      return it->second;
    }
    return defaultValue;
  }

  std::string getString(const char* key, const std::string& defaultValue = "") {
    auto it = stringValues.find(key);
    if (it != stringValues.end()) {
      return it->second;
    }
    return defaultValue;
  }

 private:
  std::map<std::string, int> intValues;
  std::map<std::string, std::string> stringValues;
};

#endif  // PREFERENCES_H