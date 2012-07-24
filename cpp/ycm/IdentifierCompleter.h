// Copyright (C) 2011, 2012  Strahinja Val Markovic  <val@markovic.io>
//
// This file is part of YouCompleteMe.
//
// YouCompleteMe is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// YouCompleteMe is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with YouCompleteMe.  If not, see <http://www.gnu.org/licenses/>.

#ifndef COMPLETER_H_7AR4UGXE
#define COMPLETER_H_7AR4UGXE

#include "ConcurrentLatestValue.h"
#include "Future.h"

#include <boost/utility.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>


namespace YouCompleteMe
{

class Candidate;
class CandidateRepository;

typedef boost::shared_ptr< std::vector< std::string > > AsyncResults;

// TODO: cpp?
typedef boost::function< std::vector< std::string >() >
  FunctionReturnsStringVector;


// TODO: move to private
// filepath -> *( *candidate )
typedef boost::unordered_map< std::string,
          boost::shared_ptr< std::list< const Candidate* > > >
            FilepathToCandidates;

// filetype -> *( filepath -> *( *candidate ) )
typedef boost::unordered_map< std::string,
          boost::shared_ptr< FilepathToCandidates > > FiletypeMap;

typedef ConcurrentLatestValue<
          boost::shared_ptr<
            boost::packaged_task< AsyncResults > > > LatestTask;

class IdentifierCompleter : boost::noncopyable
{
public:
  IdentifierCompleter();
  IdentifierCompleter( const std::vector< std::string > &candidates );
  IdentifierCompleter( const std::vector< std::string > &candidates,
             const std::string &filetype,
             const std::string &filepath );

  void EnableThreading();

  void AddCandidatesToDatabase(
      const std::vector< std::string > &new_candidates,
      const std::string &filetype,
      const std::string &filepath );

  void AddCandidatesToDatabaseFromBuffer( const std::string &buffer_contents,
                                          const std::string &filetype,
                                          const std::string &filepath );

  void ClearCandidatesStoredForFile( const std::string &filetype,
                                     const std::string &filepath );

  // Only provided for tests!
	std::vector< std::string > CandidatesForQuery(
	    const std::string &query ) const;

  std::vector< std::string > CandidatesForQueryAndType(
      const std::string &query,
      const std::string &filetype ) const;

	Future< AsyncResults > CandidatesForQueryAndTypeAsync(
	    const std::string &query,
      const std::string &filetype ) const;

private:

  void ResultsForQueryAndType( const std::string &query,
                               const std::string &filetype,
                               std::vector< Result > &results ) const;

  std::list< const Candidate* >& GetCandidateList(
      const std::string &filetype,
      const std::string &filepath );

  void InitThreads();


  /////////////////////////////
  // PRIVATE MEMBER VARIABLES
  /////////////////////////////

  CandidateRepository &candidate_repository_;

  FiletypeMap filetype_map_;

  mutable LatestTask latest_task_;

  bool threading_enabled_;

  boost::thread_group threads_;
};

} // namespace YouCompleteMe

#endif /* end of include guard: COMPLETER_H_7AR4UGXE */
