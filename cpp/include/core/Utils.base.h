/*
 * Utils.h
 *
 *  Created on: May 17, 2011
 *      Author: rleys
 */

#ifndef UTILS_H_
#define UTILS_H_


// FSMDesigner version definitions
//--------------------
#define FSMDesigner_VERSION_MAJOR @FSMDesigner_VERSION_MAJOR@
#define FSMDesigner_VERSION_MINOR @FSMDesigner_VERSION_MINOR@
#define FSMDesigner_VERSION_PATCH @FSMDesigner_VERSION_PATCH@

#define FSMDesigner_VERSION_MAJOR_STR "@FSMDesigner_VERSION_MAJOR@"
#define FSMDesigner_VERSION_MINOR_STR "@FSMDesigner_VERSION_MINOR@"
#define FSMDesigner_VERSION_PATCH_STR "@FSMDesigner_VERSION_PATCH@"

// Includes
//----------

//-- STD
#include <string>
#include <list>

using namespace std;


// Defines
//------------------
#define FOREACH_STRING_LIST(iterator_type , list_variable, element_type, element_name) \
for ( iterator_type it = (list_variable).begin(); it != (list_variable).end() ; it++) { \
element_type element_name = (*it);

#define END_FOREACH }

/**
 * This class contains utilities methods global to the software
 */
class Utils {



private:



public:
	Utils();
	virtual ~Utils();


	/**
	 * Replaces all $VAR variables by the corresponding environment VAR value
	 * @param input
	 * @param environment - The list of environment variables available
	 * @return
	 */
	static string resolveEnvironmentVariables(string input,char ** environment);

	/**
	 * Scans all environment variables and if one value matches a part of the string, replace this part by $VAR
	 * @param input
	 * @param environment - The list of environment variables available
	 * @return
	 */
	static string insertEnvironementVariables(string input,char ** environment);

	/// Represents ENV variable names to ignore when inserting environment
	/// Contains per default some useless variables like:
	/// - PWD
	/// - USER
	static list<string> getEnvironmentIgnores();

	/// Returns Major Version as set by CMake at compilation time
	static string getMajorVersion();

	/// Returns Minor Version as set by CMake at compilation time
	static string getMinorVersion();

	/// Returns Patch Version as set by CMake at compilation time
	static string getPatchVersion();


	static string itos(int i);

};

#endif /* UTILS_H_ */
