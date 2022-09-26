#include <iostream>
#include <string>

#include <jsonutils.h>
#include <utils.h>
#include <libgen.h>
#include <math.h>
#include <string.h>
#include <iostream>


int main() {
	std::string log;
	utils::sh("lsof -b -n -P -p $PPID", &log, nullptr);
	std::cout<<"lsof:\n"<<log<<std::endl;


	std::string name="aaa/", dir, base;
	utils::splitDirBasename(name, dir, base);
	std::cout<<name<<" |"<<dir<<"|"<<base<<"|"<<std::endl;

	name="/bbb";
	utils::splitDirBasename(name, dir, base);
	std::cout<<name<<" |"<<dir<<"|"<<base<<"|"<<std::endl;

	name="ccc";
	utils::splitDirBasename(name, dir, base);
	std::cout<<name<<" |"<<dir<<"|"<<base<<"|"<<std::endl;

	char buf[50];
	double pi=3.14159265359;
	double v=0;
	for (int i=0;i<10;++i) {
		snprintf(buf,sizeof(buf),"%08.3lf",v);
		std::cout<<buf<<"|\t"<<strlen(buf)<<std::endl;
		if (v==0) v=1;
		else v*=pi;

	}

	return 0;
}


