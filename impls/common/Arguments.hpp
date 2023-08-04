/*

Copyright (c) 2013 Centre for Water Systems,
                   University of Exeter

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef _CA_ARGUMENTS_HPP_
#define _CA_ARGUMENTS_HPP_

//! \file Arguments.hpp
//! Contain the class Arguments which can be used process
//! command line Argumentss.
//! \author Michele Guidolin, University of Exeter, 
//! contact: m.guidolin [at] exeter.ac.uk
//! \date 2011-07


#include"BaseTypes.hpp"
#include"Utilities.hpp"
#include<map>
#include<list>
#include<ostream>
#include<algorithm>
#include<iomanip>


namespace CA {

  //! The Arguments class contain a list of arguments and it is used
  //! to process and retrieve information from command line Arguments.
  //! An argument can be of two type, an option and a normal
  //! argument. An option is activated only when the name of the
  //! option with prefix is used in the comman line, suc as --name
  //! value. A normal argument is alway activated and must be passed
  //! at command line.

  class Arguments: public CA::Uncopyable
  {    
  public:


    //! Identify an argument.
    struct Arg
    {
      Unsigned tag;		//!< Number that is used to identify the argument.
      String   name;		//!< Name of the argument without spaces and prefix.
      String   desc;		//!< Descriptions of the argument to use in the help.
      String   value;		//!< The value of the argument, this is the output of the parse method.
      bool     option;		//!< If true, this argument is an option 
      bool     with_value;	//!< If true, the option must come with a value.
      bool     exit;	        //!< If true, exit the parsing after this option 


      Arg(Unsigned t, const String& n, const String& d, const String& v, bool o, bool w, bool e):
	tag(t), name(n), desc(d), value(v), option(o), with_value(w), exit(e) {}
      ~Arg(){}
    };
   
    //! Define a list of arguments.
    typedef std::list<Arg*> List;

    //! Define an associative container witch contains the optional
    //! argoments.
    typedef std::map<String,Arg*> Hash;


    //! Construct the object. 
    Arguments();
   

    //! Construct the object. 
    //! \param prefix The prefix used to distinguish an option from a
    //!               value.
    Arguments(const String& prefix);
   
 
    //! Destructor. 
    ~Arguments();


    //! Set the prefix.
    void setPrefix(const String& p);


    //! Clear the list of arguments and re-initilise the object as
    //! new..
    void clear();


    //! Add a possible command line argument into the Arguments
    //! list. \attention For arguments that are not an option, the order
    //! of which they are added is the order that they should appear in
    //! the command line. \warning the Tag value is not used to define
    //! the order.
    //! \param tag         The number that identify the argument.
    //! \param name        The name of the argument, without spaces and the
    //!                    prefix (also if it is an option).
    //! \param description The description of the argument.
    //! \param defvalue    The default value of the argument. In the case
    //!                    of an option, this is the value set if the
    //!                    option was activated without a value.
    //! \param option      If true the argument is an option, i.e it is
    //!                    optional and it is activated using the prefix.
    //! \param value       If the argument is an option and this parameter
    //!                    is true, the option must have a value.
    //! \param exit        If the argument is an option and this parameter
    //!                    is true, the parser must stop after parsing this option.
    void add(Unsigned tag, const String& name, const String& description, 
	     const String& defvalue, bool option = false, bool value = false, bool exit = false);
   

    //! Add a list of arguments. \warning The ownership of the
    //! arguments in the list is transfered to the calling object.
    void addList(List args);


    //! Parse the command line and populate the arguments list. If an
    //! error occur during the parsing an error message is printed in
    //! the given ouput stream and the method return false,
    bool parse(int argc, char *argv[], std::ostream& ostr);


    //! Print in the given output stream a help message on how to use
    //! the command line. 
    //! \param ostr The output stream;
    //! \param full Print the full version of the help with options
    //!             and descriptions.
    void help(std::ostream& ostr, bool full = false);


    //! Return the list of active arguments. An argument is active
    //! when it was parsed on the command line. \attention The list
    //! contains the Arg structures as pointers. Do not delete the
    //! given pointers.
    const List& active() const;


  protected:
        
    //! Functor that sort pairs by tag.
    struct SortTag {
      bool operator() (const std::pair<String,Arg*>& a, const std::pair<String,Arg*>& b)
      { 
	return (a.second->tag<b.second->tag);
      }
    };
    

  private:    

    //! The prefix that identify the optional arguments.
    String _prefix;


    //! The name of the executable.
    String _exec;


    //! Contains all the arguments. This is used to dealloc the args.
    List _all_args;

    //! Contains the arguments that are mandatory, i.e no
    //! optional. Their order in the list is the order that they are
    //! expected in the command line.
    List _args;


    //! Contains the optional arguments. This optional arguments are
    //! memorise and accessed through their name.
    Hash _options;
    

    //! Contains the arguments that were activated by the parsing.
    List _activated_args;

  };


  //! Define the type of the Options.
  typedef Arguments::List Options;


  // --- INLINE IMPLEMENTATION -- //  

  inline Arguments::Arguments():
    _prefix(),
    _exec("executable"),
    _all_args(),
    _args(),
    _options(),
    _activated_args()
  {    
  }
  

  inline Arguments::Arguments(const String& prefix):
    _prefix(prefix),
    _exec("executable"),
    _all_args(),
    _args(),
    _options(),
    _activated_args()
  {    
  }


  inline Arguments::~Arguments()
  {    
    this->clear();
  }


  inline void Arguments::setPrefix(const String& p)
  {
    _prefix = p;
  }


  inline void Arguments::clear()
  {
    // We need to deallocate all the Arg structures.
    std::for_each(_all_args.begin(), _all_args.end(), CA::Dealloc<List::value_type>());
    
    _prefix.clear();
    _exec = "executable";
    _all_args.clear();
    _args.clear();
    _options.clear();
    _activated_args.clear();
  }



  inline void Arguments::add(Unsigned tag, const String& name, const String& description, 
			     const String& defvalue, bool option, bool value, bool exit)
  {
    // Create the new Arg object and populate it with the values.
    _all_args.push_back(new Arg(tag, name, description, defvalue, option, value, exit));
    
    // If the boolean option is true add the new argument into the
    // options list, otherwise add it into the mandatory argument list.
    if(option)
    {
      _options[name] = _all_args.back();
    }
    else
    {
      _args.push_back(_all_args.back());
    }
  }


  inline void Arguments::addList(List args)
  {
    // At the moment add the arguments without doing too many
    // checks. In the future I need to check the name and tag.
    
    // Cycle throught the arguments
    for(List::const_iterator i = args.begin(); i != args.end(); ++i)
    {
      // Add new argument in the list of all arguments so it can be
      // deallocated.
      _all_args.push_back((*i));
      
      // If the boolean option is true add the new argument into the
      // options list, otherwise add it into the mandatory argument list.
      if((*i)->option)
      {
	_options[(*i)->name] = (*i);
      }
      else
      {
	_args.push_back((*i));
      }      
    }
  }
 

  inline bool Arguments::parse(int argc, char *argv[], std::ostream& ostr)
  {
    if(_prefix.empty())
    {
      ostr<<"Internal error: prefix not set"<<std::endl;
      return false;
    }
      
    // If the argv is not null, the first argument is the executable name.
    if(argv)
      _exec = argv[0];

    // Clear the list of activated arguments.
    _activated_args.clear();
      
    // Check that the number of argument minus one (the exec name) is
    // at least as the same number of mandatory arguments.
    //if( static_cast<Unsigned>(argc-1) < _args.size() )
    //{
    //  help(ostr, false);
    //  return false;
    //}
  
    // Iterator to the current mandatory argument that need to be activated.
    List::iterator i_arg = _args.begin();

	// NOTE: the correction here, as this system takes all arguements, and doesn't assume the exec name is provided.


    // Cycle through each command line arguments after the first one.
    for(int i_tok = 1; i_tok<argc; i_tok++)
	/////for(int i_tok = 0; i_tok<argc; i_tok++)
    {
      // Check if the argument token has the prefix
      if(CA::compareCaseInsensitive(_prefix,argv[i_tok],true))
      {
	// This is an option. Search the option into the associative
	// container where the name of the option start from the size of
	// the prefix.
	Hash::iterator i_opt = _options.find(&argv[i_tok][_prefix.size()]);

	// If the option is not found return an error. 
	if(i_opt == _options.end())
	{
	  ostr<<"Error: option not recognised '"<<argv[i_tok]<<"'"<<std::endl;
	  return false;
	}

	// Add the option to the list of activated arguments.
	_activated_args.push_back((*i_opt).second);
	    
	// Check if the option must have a value.
	if((*i_opt).second->with_value)
	{
	  // The value is the next token, thus increase the toke index
	  // and check if there is another token available.
	  ++i_tok;
	  if(i_tok==argc)
	  {
	    ostr<<"Error: expected a value after option'"<<argv[i_tok-1]<<"'"<<std::endl;
	    return false;
	  }       

	  // Finally retrieve the value of the option.
	  (*i_opt).second->value = argv[i_tok];
	}

	// Stop the parsing if the option has the exit boolean set.
	if((*i_opt).second->exit)
	  return true;
      }
      else
      {
	// This is an argument. Thus if the iterator to the current
	// mandatory arguments is at the end of the list, there are
	// too many arguments passed.
	if(i_arg == _args.end())
	{
	  ostr<<"Error: too many arguments passed at command line, from argument '"<<argv[i_tok]<<"'"<<std::endl;
	  help(ostr, false);
	  return false;
	}

	// Add the argument pointer into the lits of activated
	// arguments.
	_activated_args.push_back((*i_arg));
      
	// Set the given argument value to the be equal to the command
	// line argument. Then pass to the next argument.
	(*i_arg)->value = argv[i_tok];
	++i_arg;     
      }
    }

    // If the current mandatory argument iterator is not at the end of
    // the list, the command line was missing some arguments.
    if(i_arg != _args.end())
    {
      ostr<<"Error: not enough arguments passed at command line, missing <"<<(*i_arg)->name<<">"<<std::endl;
      help(ostr, false);
      return false;
    }

    return true;
  }


  inline void Arguments::help(std::ostream& ostr, bool full)
  {
    // Print first part of the help with the name of the executable ...
    ostr<<"Usage: "<<_exec<<" [options]";

    // .. and all the mandatory arguments.
    for(List::const_iterator i = _args.begin(); i != _args.end();  ++i)
    {
      ostr<<" <"<<(*i)->name<<">";
    }

    // End line
    ostr<<std::endl;

    // Now stop if the help does not include the full descriptions.
    if(full == false)
      return;

    // Full desctiption of each argument.
    ostr<<std::endl<<"Mandatory arguments:"<<std::endl;
  
    int tab = 50;
    ostr<<std::setfill('.');

    for(List::const_iterator i = _args.begin(); i != _args.end();  ++i)
    {
      ostr<<" "<<std::setw(tab)<<std::left<<(*i)->name+" "<<" "<<(*i)->desc<<std::endl;
    }

    // Full desctiption of each option.
    ostr<<std::endl<<"Options:"<<std::endl;

    // Copy the options into a temporary vector that is going to be
    // sorted by tag.
    std::vector< std::pair<String, Arg*> > tmpv(_options.begin(), _options.end());    
    std::sort (tmpv.begin(), tmpv.end(), SortTag());  
  
    for(std::vector< std::pair<String, Arg*> >::const_iterator i = tmpv.begin(); i != tmpv.end();  ++i)
    {
      ostr<<" "<<std::setw(tab)<<std::left<<_prefix+(*i).second->name+" "<<" "<<(*i).second->desc<<std::endl;
    }

    ostr<<std::endl;
    ostr<<std::setfill(' ');
  }
  

  inline const Arguments::List& Arguments::active() const
  {
    return _activated_args;
  }

} // Namespace CA

#endif // _CA_ARGUMENTS_HPP_	  
