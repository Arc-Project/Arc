#include "Utility.h"

/*---------------------------------------------------------------------------------------------------------------------
-- FUNCTION: split
--
-- DATE: April 11, 2016
--
-- REVISIONS: (none)
--
-- DESIGNER: Hank Lo
--
-- PROGRAMMER: Hank Lo
--
-- INTERFACE: std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
--		const std::string &s: The string we want to split up
--		char delim: The delimiter we want to split the string with
--		std::vector<std::string> &elems: The vector where the split up strings will be stored
--
-- RETURNS: a preconstructed std::vector<std::string>, filled by this function.
--
-- NOTES:
-- This function splits a standard string up using the provided character delimiter, and stores the elements into the provided
-- vector of strings.
---------------------------------------------------------------------------------------------------------------------*/
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

/*---------------------------------------------------------------------------------------------------------------------
-- FUNCTION: split
--
-- DATE: April 11, 2016
--
-- REVISIONS: (none)
--
-- DESIGNER: Hank Lo
--
-- PROGRAMMER: Hank Lo
--
-- INTERFACE: std::vector<std::string> split(const std::string &s, char delim)
--		const std::string &s: The string you want to split up
--		char delim: the delimiter you want to split the string with
--
-- RETURNS: a new vector of strings, each part of the vector representing a portion of the original string.
--
-- NOTES:
-- This function takes a string and a delimiter and returns a vector of strings, in which each element is a part of the
-- original string. This function is a wrapper function that calls the other split function so that the user does not
-- have to manually create a vector.
---------------------------------------------------------------------------------------------------------------------*/
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}


