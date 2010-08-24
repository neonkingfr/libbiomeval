/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/
#ifndef __BE_IO_LOGCABINET_H
#define __BE_IO_LOGCABINET_H

#include <sstream>
#include <string>
#include <vector>

#include <stdio.h>
#include <be_error_exception.h>
using namespace std;

/*
 * This file contains the class declaration for the LogCabinet, a class
 * that represents a collection of log sheets, and LogSheet, a class that
 * represents one log file.
 */
namespace BiometricEvaluation {
    namespace IO {

	/*
	 * Class to represent a single logging mechanism. A LogSheet is
	 * a string stream, so applications can write into the stream as
	 * a staging area using the << operator, then start a new entry by
	 * calling newEntry(). Entries in the log file are prefixed with an
	 * entry number, which is incremented when the entry is written
	 * (either by directly calling write(), or calling newEntry()).
	 *
	 * A LogSheet object can be constructed and passed back to the client
	 * by the LogCabinet object. All sheets created in the manner are
	 * placed in a common area maintained by the cabinet.
	*/
	class LogSheet : public std::ostringstream {
		public:
			/*
			 * Create a new log sheet.
			 * Parameters:
			 * 	name (in)
			 *		The name of the LogSheet to be
			 *		created.
			 *	description (in)
			 *		The text used to describe the sheet.
			 *		This text is written into the log file
			 *		prior to any entries.
			 *	parentDir (in)
			 *		Where, in the file system, the sheet
			 *		is to be stored. This directory must
			 *		exist.
			 * Returns:
			 *	An object representing the new log sheet.
			 * Throws:
			 * 	Error::ObjectExists
			 *		The sheet was previously created.
			 *	Error::StrategyError
			 *		An error occurred when using the
			 *		underlying file system, or the name
			 *		is malformed.
			 */
			LogSheet(const string &name,
				 const string &description,
				 const string &parentDir)
			    throw (Error::ObjectExists, Error::StrategyError);

			~LogSheet();

			/*
			 * Write a string as an entry to the log file. This
			 * does not affect the current log entry buffer, but
			 * does increment the entry number.
			 * Parameters:
			 *	entry (in)
			 *		The text of the log entry.
			 *
			 * Throws:
			 *	Error::StrategyError
			 *		An error occurred when using the
			 *		underlying file system.
			 */
			void write(const string &entry)
			    throw (Error::StrategyError);

			/*
			 * Start a new entry, causing the existing entry
			 * to be closed. Applications do not have to call
			 * this method for the first entry, however, as the
			 * stream is ready for writing upon construction.
			 * Throws:
			 *	Error::StrategyError
			 *		An error occurred when using the
			 *		underlying file system.
			 */
			void newEntry()
			    throw (Error::StrategyError);

			/*
			 * Return the contents of the current entry currently
			 * under construction.
			 * Returns:
			 *	The text of the current entry.
			 */
			string getCurrentEntry();

			/*
			 * Reset the current entry buffer to the beginning.
			 */
			void resetCurrentEntry();

			/*
			 * Return the current entry number.
			 */
			uint32_t getCurrentEntryNumber();

			/*
			 * Synchronize any buffered data to the underlying
			 * log file. This syncing is dependent on the behavior
			 * of the underlying filesystem/OS.
			 * Throws:
			 *	Error::StrategyError
			 *		An error occurred when using the
			 *		underlying file system.
			*/
			void sync()
			    throw (Error::StrategyError);

			/*
			 * Turn on/off auto-sync of the data. When TRUE, the
			 * data is sync'd whenever newEntry() is or write()
			 * is called.
			 * Parameters:
			 * 	state
			 *		TRUE if auto-sync is desired, FALSE
			 *		otherwise.
			*/
			void setAutoSync(bool state);

		private:
			uint32_t _entryNumber;
			FILE *_theLogFile;
			bool _autoSync;
	};

	/*
	 * Class to represent a collection of log sheets.
	 */
	class LogCabinet {
		public:

			/*
			 * Create a new LogCabinet in the file system.
			 * Parameters:
			 * 	name (in)
			 *		The name of the LogCabinet to be
			 *		created.
			 *	description (in)
			 *		The text used to describe the cabinet.
			 *	parentDir (in)
			 *		Where, in the file system, the cabinet
			 *		is to be stored. This directory must
			 *		exist.
			 * Returns:
			 *	An object representing the new log cabinet.
			 * Throws:
			 * 	Error::ObjectExists
			 *		The cabinet was previously created.
			 *	Error::StrategyError
			 *		An error occurred when using the
			 *		underlying file system, or the name
			 *		is malformed.
			*/
			LogCabinet(
			    const string &name,
			    const string &description,
			    const string &parentDir)
			    throw (Error::ObjectExists, Error::StrategyError);

			/*
			 * Open an existing LogCabinet.
			 * Parameters:
			 * 	name (in)
			 *		The name of the LogCabinet to be opened.
			 *	parentDir (in)
			 *		Where, in the file system, the cabinet
			 *		is stored.
			 * Returns:
			 *	An object representing the existing log cabinet.
			 * Throws:
			 * 	Error::ObjectDoesNotExist
			 *		The cabinet does not exist in the
			 *		file system.
			 *	Error::StrategyError
			 *		An error occurred when using the
			 *		underlying file system, or the name
			 *		is malformed.
			 */
			LogCabinet(
			    const string &name,
			    const string &parentDir)
			    throw (Error::ObjectDoesNotExist, 
			    Error::StrategyError);

			~LogCabinet();
			
			/*
			 * Create a new LogSheet within the LogCabinet.
			 * Parameters:
			 * 	name (in)
			 *		The name of the LogSheet to be
			 *		created.
			 *	description (in)
			 *		The text used to describe the sheet.
			 *		This text is written into the log file
			 *		prior to any entries.
			 * Returns:
			 *	A pointer to the new log sheet.
			 * Throws:
			 * 	Error::ObjectExists
			 *		The sheet was previously created.
			 *	Error::StrategyError
			 *		An error occurred when using the
			 *		underlying file system, or the name
			 *		is malformed.
			*/
			LogSheet *newLogSheet(
			    const string &name,
			    const string &description)
			    throw (Error::ObjectExists, Error::StrategyError);

			/* Return the name of the LogCabinet */
			string getName();

			/* Return a textual description of the LogCabinet */
			string getDescription();

			/* Return the number of items in the LogCabinet */
			unsigned int getCount();

			/*
			 * Remove a LogCabinet.
			 * Parameters:
			 * 	name (in)
			 *		The name of the LogCabinet to be
			 *		removed.
			 *	parentDir (in)
			 *		Where, in the file system, the cabinet
			 *		is stored.
			 * Throws:
			 * 	Error::ObjectDoesNotExist
			 *		The cabinet does not exist in the
			 *		file system.
			 *	Error::StrategyError
			 *		An error occurred when using the
			 *		underlying file system, or the name
			 *		is malformed.
			 */
			static void remove(
			    const string &name,
			    const string &parentDir)
			    throw (Error::ObjectDoesNotExist, 
			    Error::StrategyError);

		protected:
			/*
			 * The data members of the LogCabinet are protected 
			 * so derived classes can use them while still being
			 * hidden from non-derived classes.
			 */
			 
			/* The name of the LogCabinet */
			string _name;

			/* The parent directory of the cabinet */
			string _parentDir;

			/* The directory where the cabinet is rooted */
			string _directory;

			/* A textual description of the cabinet. */
			string _description;

			/* Number of items in the cabinet */
			unsigned int _count;

			/* The current record position cursor */
			int _cursor;

			/*
			 * Return the full path of a file stored as part
			 * of the LogCabinet, typically _directory + name.
			 */
			string canonicalName(const string &name);

			/* Read the contents of the common control file format
			 * for all LogCabinet.
			 */
			void readControlFile() throw (Error::StrategyError);

			/* Write the contents of the common control file format
			 * for all LogCabinet.
			 */
			void writeControlFile() throw (Error::StrategyError);

	};
    }
}
#endif	/* __BE_IO_LOGCABINET_H */
