#ifndef SRC_JSONUTILS_H_
#define SRC_JSONUTILS_H_

#include <memory>
#include <ostream>
#include <string>
#include <stdexcept>
#include <jansson.h>


namespace json {
	typedef std::shared_ptr<json_t> jsonptr;
	std::ostream& operator<<(std::ostream& out, const json_t*);
	std::ostream& operator<<(std::ostream& out, const jsonptr & p);
}
namespace {
	inline std::ostream& operator<<(std::ostream& out, const json_t* j) {
		return json::operator<<(out, j);
	}
	inline std::ostream& operator<<(std::ostream& out, const std::shared_ptr<json_t> & j) {
		return json::operator<<(out, j);
	}
}

namespace json {

std::string to_string(const json_t *p);
inline std::string to_string(const jsonptr & p) {
	return to_string(p.get());
}

std::ostream& operator<<(std::ostream& out, const json_t* j);
std::ostream& operator<<(std::ostream& out, const json::jsonptr & j);


inline jsonptr own(json_t* n) {return jsonptr(n, json_decref);}
inline jsonptr attach(json_t* n) {return jsonptr(json_incref(n), json_decref);}

extern jsonptr parse(const char* p);
extern jsonptr parse(const char* p, size_t size);
extern jsonptr parse(const std::string & str);

extern std::string pretty(const json_t* c);
inline std::string pretty(const jsonptr & c) {return pretty(c.get());}

inline void collectPath(std::string & str) {}
template<typename...REST> void collectPath(std::string & str, const char* first, REST... rest) {
	str+=first;
	if (sizeof...(rest) > 0) str+=".";
	collectPath(str,rest...);
}

inline const char* getString(const json_t* c) {return (c && json_is_string(c)) ? json_string_value(c) : nullptr;}
inline const char* getString(const jsonptr & c) {return getString(c.get());}
template<typename...REST> const char* getString(const json_t* p, const char* first, REST... rest) {
	if (!p) return nullptr;
	auto c=json_object_get(p,first);
	return getString(c,rest...);
}
template<typename...REST> const char* getString(const jsonptr& p, const char* first, REST... rest) {
	return getString(p.get(),first,rest...);
}
template<typename...REST> const char* getString(const char* fallback,const json_t* p, const char* first, REST... rest) {
	auto ret=getString(p,first,rest...);
	return ret?ret:fallback;
}
template<typename...REST> const char* getString(const char* fallback,const jsonptr& p, const char* first, REST... rest) {
	return getString(fallback,p.get(),first,rest...);
}
inline bool hasString(const json_t* c) {return c && json_is_string(c);}
template<typename...REST> bool hasString(const json_t* p, const char* first, REST... rest) {
	if (!p) return false;
	auto c=json_object_get(p,first);
	return hasString(c, rest...);
}
template<typename...REST> bool hasString(const jsonptr& p, REST... rest) {
	return hasString(p.get(),rest...);
}

template<typename...REST> const char* getStringOrThrow(const std::string& msg, const json_t * p,  REST... rest) {
	if (!hasString(p,rest...)) {
		std::string path;
		collectPath(path,rest...);
		throw std::runtime_error(msg+" "+path+" is not properly defined in "+pretty(p));
	}
	return getString(p, rest...);
}
template<typename...REST> const char*  getStringOrThrow(const std::string& msg, const jsonptr& p,  REST... rest) {
	return getStringOrThrow(msg,p.get(),rest...);
}

inline long long getLong(const json_t* c) {return (c && json_is_integer(c))?json_integer_value(c):0;}
inline long long getLong(long long fallback, const json_t* c) {return (c && json_is_integer(c))?json_integer_value(c):fallback;}
template<typename...REST> long long getLong(const json_t* p, const char* first, REST... rest) {
	if (!p) return 0;
	auto c=json_object_get(p,first);
	return getLong(c,rest...);
}
template<typename...REST> long long getLong(const jsonptr& p,  REST... rest) {
	return getLong(p.get(),rest...);
}
template<typename...REST> long long getLong(long long fallback, const json_t* p, const char* first, REST... rest) {
	if (!p) return fallback;
	auto c=json_object_get(p,first);
	return getLong(fallback,c,rest...);
}
template<typename...REST> long long getLong(long long fallback, const jsonptr& p,  REST... rest) {
	return getLong(fallback,p.get(),rest...);
}
inline bool hasLong(const json_t* c) {return c && json_is_integer(c);}
template<typename...REST> bool hasLong(const json_t* p, const char* first, REST... rest) {
	if (!p) return false;
	auto c=json_object_get(p,first);
	return hasLong(c, rest...);
}
template<typename...REST> bool hasLong(const jsonptr& p,REST... rest) {
	return hasLong(p.get(),rest...);
}
inline std::shared_ptr<long long> getLongPtr(const json_t* c) {
	return (c && json_is_integer(c))?std::make_shared<long long>(json_integer_value(c)):std::shared_ptr<long long>();
}
template<typename...REST> std::shared_ptr<long long> getLongPtr(const json_t* p, const char* first, REST... rest) {
	if (!p) return std::make_shared<long long>();
	auto c=json_object_get(p,first);
	return getLong(c,rest...);
}
template<typename...REST> std::shared_ptr<long long> getLongPtr(const jsonptr & p, REST... rest) {
	return getLongPtr(p.get(),rest...);
}

template<typename...REST> long long getLongOrThrow(const std::string& msg, const json_t* p,  REST... rest) {
	if (!hasLong(p,rest...)) {
		std::string path;
		collectPath(path,rest...);
		throw std::runtime_error(msg+""+path+" is not properly defined in "+pretty(p));
	}
	return getLong(p, rest...);
}
template<typename...REST> long long getLongOrThrow(const std::string& msg, const jsonptr& p,  REST... rest) {
	return getLongOrThrow( msg, p.get(), rest...) ;
}

inline bool getBool(const json_t* c) {return (c && json_is_boolean(c))?json_boolean_value(c):0;}
inline bool getBool(bool fallback, const json_t* c) {return (c && json_is_boolean(c))?json_boolean_value(c):fallback;}
template<typename...REST> bool getBool(const json_t* p, const char* first, REST... rest) {
	if (!p) return false;
	auto c=json_object_get(p,first);
	return getBool(c,rest...);
}
template<typename...REST> bool getBool(bool fallback, const json_t* p, const char* first, REST... rest) {
	if (!p) return fallback;
	auto c=json_object_get(p,first);
	return getBool(fallback,c,rest...);
}
template<typename...REST> bool getBool(bool fallback, const jsonptr& p,  REST... rest) {
	return getBool(fallback,p.get(),rest...);
}
inline bool hasBool(const json_t* c) {return c && json_is_boolean(c);}
template<typename...REST> bool hasBool(const json_t* p, const char* first, REST... rest) {
	if (!p) return false;
	auto c=json_object_get(p,first);
	return hasBool(c, rest...);
}
template<typename...REST> bool hasBool(const jsonptr& p,REST... rest) {
	return hasBool(p.get(),rest...);
}
inline std::shared_ptr<bool> getBoolPtr(const json_t* c) {
	return (c && json_is_boolean(c))?std::make_shared<bool>(json_boolean_value(c)) : std::make_shared<bool>();
}
template<typename...REST> std::shared_ptr<bool> getBoolPtr(const json_t* p, const char* first, REST... rest) {
	if (!p) return std::make_shared<bool>();
	auto c=json_object_get(p,first);
	return getBool(c,rest...);
}
template<typename...REST> std::shared_ptr<bool> getBoolPtr(const jsonptr & p, REST... rest) {
	return getBoolPtr(p.get(),rest...);
}



inline double getNumber(const json_t* c) {return (c && json_is_number(c))?json_number_value(c):0;}
inline double getNumber(bool fallback, const json_t* c) {return (c && json_is_number(c))?json_number_value(c):fallback;}
template<typename...REST> double getNumber(const json_t* p, const char* first, REST... rest) {
	if (!p) return 0;
	auto c=json_object_get(p,first);
	return getNumber(c,rest...);
}
template<typename...REST> double getNumber(bool fallback, const json_t* p, const char* first, REST... rest) {
	if (!p) return fallback;
	auto c=json_object_get(p,first);
	return getNumber(fallback,c,rest...);
}
template<typename...REST> double getNumber(bool fallback, const jsonptr& p,  REST... rest) {
	return getNumber(fallback,p.get(),rest...);
}
inline bool hasNumber(const json_t* c) {return c && json_is_number(c);}
template<typename...REST> bool hasNumber(const json_t* p, const char* first, REST... rest) {
	if (!p) return false;
	auto c=json_object_get(p,first);
	return hasNumber(c, rest...);
}
template<typename...REST> bool hasNumber(const jsonptr& p,REST... rest) {
	return hasNumber(p.get(),rest...);
}
inline std::shared_ptr<double> getNumberPtr(const json_t* c) {
	return (c && json_is_number(c))?std::make_shared<double>(json_number_value(c)):std::make_shared<double>();
}
template<typename...REST> std::shared_ptr<double> getNumberPtr(const json_t* p, const char* first, REST... rest) {
	if (!p) return std::make_shared<double>();
	auto c=json_object_get(p,first);
	return getNumber(c,rest...);
}
template<typename...REST> std::shared_ptr<double> getNumberPtr(const jsonptr & p, REST... rest) {
	return getNumberPtr(p.get(),rest...);
}
template<typename...REST> double getNumberOrThrow(const std::string& msg, const json_t* p,  REST... rest) {
	if (!hasNumber(p,rest...)) {
		std::string path;
		collectPath(path,rest...);
		throw std::runtime_error(msg+""+path+" is not properly defined in "+pretty(p));
	}
	return getNumber(p, rest...);
}
template<typename...REST> double getNumberOrThrow(const std::string& msg, const jsonptr& p,  REST... rest) {
	return getNumberOrThrow( msg, p.get(), rest...) ;
}



inline json_t* getChild(json_t* c) {return c;}
inline json_t* getChild(json_t* fallback, json_t* c) {return c?c:fallback;}
template<typename...REST> json_t* getChild(const json_t* p, const char* first, REST... rest) {
	if (!p) return nullptr;
	auto c=json_object_get(p,first);
	return getChild(c,rest...);
}
template<typename...REST> json_t* getChild(const jsonptr& p,  REST... rest) {
	return getChild(p.get(),rest...);
}
template<typename...REST> json_t* getChild(json_t* fallback, const json_t* p, const char* first, REST... rest) {
	if (!p) return fallback;
	auto c=json_object_get(p,first);
	return getChild(fallback,c,rest...);
}
template<typename...REST> json_t* getChild(json_t* fallback, const jsonptr& p,  REST... rest) {
	return getChild(fallback,p.get(),rest...);
}
inline bool hasChild(const json_t* c) {return c!=nullptr;}
template<typename...REST> bool hasChild(const json_t* p, const char* first, REST... rest) {
	if (!p) return false;
	auto c=json_object_get(p,first);
	return hasChild(c, rest...);
}
template<typename...REST> bool hasChild(const jsonptr& p,REST... rest) {
	return hasChild(p.get(),rest...);
}
inline jsonptr getChildPtr(json_t* c) {
	return attach(c);
}
template<typename...REST> jsonptr getChildPtr(const json_t* p, const char* first, REST... rest) {
	if (!p) return jsonptr();
	auto c=json_object_get(p,first);
	return getChild(c,rest...);
}
template<typename...REST> jsonptr getChildPtr(const jsonptr & p, REST... rest) {
	return getChildPtr(p.get(),rest...);
}

template<typename...REST> json_t* getChildOrThrow(const std::string& msg, const json_t* p,  REST... rest) {
	if (!hasChild(p,rest...)) {
		std::string path;
		collectPath(path,rest...);
		throw std::runtime_error(msg+""+path+" is not properly defined in "+pretty(p));
	}
	return getChild(p, rest...);
}
template<typename...REST> json_t* getChildOrThrow(const std::string& msg, const jsonptr& p,  REST... rest) {
	return getChildOrThrow( msg, p.get(), rest...) ;
}

template<typename...REST> jsonptr getChildPtrOrThrow(const std::string& msg, const json_t* p,  REST... rest) {
	return attach(getChildOrThrow(msg, p, rest...));
}
template<typename...REST> jsonptr getChildPtrOrThrow(const std::string& msg, const jsonptr& p,  REST... rest) {
	return getChildPtrOrThrow(msg, p.get(), rest...);
}




class JsonKeyValueIterator {
	const json_t* obj;
	void* ptr;
public:
	explicit JsonKeyValueIterator() : ptr(nullptr) {}
	explicit JsonKeyValueIterator(const json_t* p) : obj(p), ptr(json_object_iter((json_t*)p)) {}
	explicit JsonKeyValueIterator(const jsonptr& p) : JsonKeyValueIterator(p.get()) {}
	inline bool operator !=(JsonKeyValueIterator rhs) const {return ptr!=rhs.ptr;}
	inline std::pair<const char*, json_t*> operator *() {
		const char* key=json_object_iter_key(ptr);
		json_t* val=json_object_iter_value(ptr);
		return std::make_pair(key,val);
	}
	inline void operator++() {ptr=json_object_iter_next((json_t*)obj, ptr);}
};
class JsonKeyValuePairs {
	const json_t* obj;
public:
	explicit JsonKeyValuePairs(const json_t* p) : obj(p) {}
	explicit JsonKeyValuePairs(const jsonptr& p) : JsonKeyValuePairs(p.get()) {}
	inline JsonKeyValueIterator begin() const {return JsonKeyValueIterator(obj);}
	inline JsonKeyValueIterator end() const {return JsonKeyValueIterator();}
};
inline JsonKeyValuePairs getJsonKeyValuePairs(const json_t* p) {return JsonKeyValuePairs(p);}
template<typename...REST> JsonKeyValuePairs getJsonKeyValuePairs(const json_t* p,const char* first, REST... rest) {
	return getJsonKeyValuePairs(json_object_get(p,first), rest...);
}

class JsonKeyIterator {
	const json_t* obj;
	void* ptr;
public:
	explicit JsonKeyIterator() : ptr(nullptr) {}
	explicit JsonKeyIterator(const json_t* p) : obj(p), ptr(json_object_iter((json_t*)p)) {}
	explicit JsonKeyIterator(const jsonptr& p) : JsonKeyIterator(p.get()) {}
	inline bool operator !=(JsonKeyIterator rhs) const {return ptr!=rhs.ptr;}
	inline const char* operator *() const {return json_object_iter_key(ptr);}
	inline void operator++() {ptr=json_object_iter_next((json_t*)obj, ptr);}
};

class JsonKeys {
	const json_t* obj;
public:
	explicit JsonKeys(const json_t* p) : obj(p) {}
	explicit JsonKeys(const jsonptr& p) : obj(p.get()) {}
	inline JsonKeyIterator begin() const {return JsonKeyIterator(obj);}
	inline JsonKeyIterator end() const {return JsonKeyIterator();}
};
inline JsonKeys getJsonKeys(const json_t* p) {return JsonKeys(p);}
template<typename...REST> JsonKeys getJsonKeys(const json_t* p,const char* first, REST... rest) {
	return getJsonKeys(json_object_get(p,first), rest...);
}
template<typename...REST> JsonKeys getJsonKeys(const jsonptr& p, REST... rest) {
	return getJsonKeys(p.get(), rest...);
}

class JsonArrayIterator {
	const json_t* array;
	size_t pos;
public:
	explicit JsonArrayIterator(const json_t* p) : array(p),pos(0) {}
	explicit JsonArrayIterator(const json_t* p,size_t s) : array(p),pos(s) {}
	inline bool operator ==(JsonArrayIterator rhs) const {return pos==rhs.pos && array==rhs.array;}
	inline bool operator !=(JsonArrayIterator rhs) const {return ! (rhs== *this);}
	inline json_t* operator *() const {return json_array_get(array, pos);}
	inline void operator++() {pos++;}
	inline void operator--() {pos--;}
	inline operator bool() const {return pos>=0 && pos<json_array_size(array);}
};

class JsonArrayElements {
	const json_t* array;
	size_t size;
public:
	explicit JsonArrayElements(const json_t* p) : array(p),size(json_array_size(p)) {}
	inline JsonArrayIterator begin() const {return JsonArrayIterator(array);}
	inline JsonArrayIterator end() const {return JsonArrayIterator(array,size);}
};

inline JsonArrayElements getJsonArrayElements(const json_t* p) {return JsonArrayElements(p);}
template<typename...REST> JsonArrayElements getJsonArrayElements(const json_t* p,const char* first, REST... rest) {
	return getJsonArrayElements(json_object_get(p,first), rest...);
}
template<typename...REST> JsonArrayElements getJsonArrayElements(const jsonptr& p,REST... rest) {
	return getJsonArrayElements(p.get(),rest...);
}


}
#endif /* SRC_JSONUTILS_H_ */
