#include <jsonutils.h>
#include <iomanip>

namespace json {

std::string to_string(const json_t *p) {
	if (p) {
		std::shared_ptr<char> s(json_dumps(p, JSON_COMPACT | JSON_ENCODE_ANY) ,::free);
		return std::string(s.get());
	} else {
		return std::string("null");
	}
}
std::ostream& operator<<(std::ostream& out, const json_t* p) {
	if (p) {
		std::shared_ptr<char> s(json_dumps(p, JSON_COMPACT | JSON_ENCODE_ANY) ,::free);
		out<<s.get();
	} else {
		out<<"null";
	}
	return out;
}
std::ostream& operator<<(std::ostream& out, const jsonptr & p) {
	out<<p.get();
	return out;
}

jsonptr parse(const char* p) {
	if (!p) return jsonptr();
	json_error_t e;
	auto l=json_loads(p, JSON_DECODE_ANY, &e);
	if (!l) {
		throw std::runtime_error(std::string("Invalid json: ")+
				std::string(p)+
				"; Error: "+
				std::string(e.text)+
				std::string(": line : ")+std::to_string(e.line)+
				std::string(", column: ")+std::to_string(e.column)+
				std::string(", position: ")+std::to_string(e.position)
				);
	}
	return own(l);
}

jsonptr parse(const char* p, size_t len) {
	if (!p || len==0) return jsonptr();
	json_error_t e;
	auto l=json_loadb(p, len, JSON_DECODE_ANY, &e);
	if (!l) {
		throw std::runtime_error(std::string("Invalid json: ")+
				std::string(p, p+len)+
				"; Error: "+
				std::string(e.text)+
				std::string(": line : ")+std::to_string(e.line)+
				std::string(", column: ")+std::to_string(e.column)+
				std::string(", position: ")+std::to_string(e.position)
				);
	}
	return own(l);
}
jsonptr parse(const std::string& str) {
	return parse(str.c_str(),str.length());
}
std::string pretty(const json_t* j, int off);
std::string pretty(const json_t* j) {
	return  j ? pretty(j, 0) : "null";
}
std::string pretty(const json_t* j, int off) {
	switch (j->type) {
		case JSON_OBJECT: {
			std::string ret="{\n";
			size_t max=json_object_size(j);
			size_t i=0;
			for (auto p : getJsonKeyValuePairs(j)) {
				auto pk=to_string(own(json_string(p.first)));
				auto pv=pretty(p.second, off+1);
				ret+=std::string(off+1, '\t');
				ret+=pk;
				ret+=" : ";
				ret+=pv;
				++i;
				if (i< max) ret+=",";
				ret+="\n";
			}
			ret+=std::string(off,'\t');
			ret+="}";
			return std::move(ret);
		}
		case JSON_ARRAY: {
			std::string ret="[\n";
			size_t max=json_array_size(j);
			size_t i=0;
			for (auto p : getJsonArrayElements(j)) {
				auto pv=pretty(p, off+1);
				ret+=std::string(off+1, '\t');
				ret+=pv;
				++i;
				if (i< max) ret+=",";
				ret+="\n";
			}
			ret+=std::string(off,'\t');
			ret+="]";
			return std::move(ret);
		}
		case JSON_STRING: {
			return to_string(j);
		}
		case JSON_INTEGER: {
			return std::to_string(json_integer_value(j));
		}
		case JSON_REAL: {
			std::stringstream ss;
			ss<<std::fixed<<json_real_value(j);
			std::string ret;
			ss>>ret;
			ssize_t last=ret.length()-1;
			if (ret.at(last)=='0') {
				bool stillZero=true;
				ssize_t lastZero=last;
				ssize_t dotPos=-1;
				for (ssize_t i=last-1;i>=0;--i) {
					char c=ret.at(i);
					if (stillZero) {
						if (c=='0') {
							lastZero=i;
							continue;
						} else stillZero=false;
					}
					if (c=='.') {
						dotPos=i;
						break;
					}
				}
				if (dotPos!=-1) {
					if (lastZero==dotPos+1) ++lastZero;
					if (lastZero<=last) ret.erase(lastZero,std::string::npos);
				}
			}
			return std::move(ret);
		}
		case JSON_TRUE: {
			return "true";
		}
		case JSON_FALSE: {
			return "false";
		}
		case JSON_NULL: {
			return "null";
		}
		default:
			throw std::runtime_error("Unknown json element type: "+std::to_string(j->type));
	}
}

}
