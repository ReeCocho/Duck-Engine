#pragma once

/**
 * @file debugging.hpp
 * @brief Debugging utilities.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <iostream>

/** Print a message to the console in both debugging and release builds. */
#define dk_log(MSG) std::clog << MSG

/** Assert that a condition is true at compile time and stop compiling if it's false. */
#define dk_static_assert(COND) static_assert(COND)

#ifdef NDEBUG
	#define dk_err(MSG) ((void)0)
	#define dk_out(MSG) ((void)0)
	#define dk_assert(COND) ((void)0)
#else
	/** Print an error and terminate the program. */
	#define dk_err(MSG) { std::cerr << MSG; std::terminate(); }((void)0)
	
	/** Print a message to the console only in the debug build. */
	#define dk_out(MSG) { std::cout << MSG }((void)0)
	
	/** Assert that a condition is true and terminate the program if it's false. */
	#define dk_assert(COND) {if(!(COND)) dk_err("Assertation failed in file " << __FILE__ << " on line " << __LINE__  << " : " << #COND << '\n'); }((void)0)
#endif