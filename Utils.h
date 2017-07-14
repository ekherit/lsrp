/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  Some utils
 *
 *        Version:  1.0
 *        Created:  11.07.2017 16:35:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (ekherit), I.B.Nikolaev@inp.nsk.su
 *   Organization:  Budker Insitute of Nuclear Physics
 *
 * =====================================================================================
 */
#pragma once 

#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

inline long GetPhysMemUsage(void)
{ //Note: this value is in KB!
  FILE* file = fopen("/proc/self/status", "r");
  long result = -1;
  char line[128];

  while (fgets(line, 128, file) != NULL)
  {
    if (strncmp(line, "VmRSS:", 6) == 0)
    {
      sscanf(line,"VmRSS: %ld", & result);
      break;
    }
  }
  fclose(file);
  return result;
}

inline void print_memory_usage(long run = 0)
{
  std::ofstream mem_usage("mem_usage.txt",std::ios_base::app);
  mem_usage << "MEMORY USAGE: " << run << ": " << GetPhysMemUsage() << " kB" << std::endl;
}


/*
 * =====================================================================================
 *        Class:  print_predicate
 *  Description:  Helper class to print event accordint to list of denominators
 *  if (print_predicate(event, {1,10, 33}))
 *  {
 *      print every 1 for event less 10
 *      then every 10 for event less  33
 *      then every 33
 *  }
 * =====================================================================================
 */
class print_predicate
{
  public:
    template<typename Number>
    print_predicate(Number event, const std::vector<Number> && denom) 
    {
      for(size_t i=0; i<denom.size();i++)
      {
        isprint  = isprint || ( (event < denom[i+1] || i==(denom.size()-1) ) && (event % denom[i] == 0) );
      }
    }

    template<typename Number, typename ...Types >
    print_predicate(Number event, Types ...args) :  print_predicate(event, {args...})
    {
    }

    bool operator()(void) const noexcept{ return isprint; }
    explicit operator bool() const noexcept { return isprint; }
  private:
    bool isprint=false;
};

class column_printer
{
  public:
    template<typename ...Cols>
    column_printer(Cols ... cols) : v{cols...}
    {
      sum = 0;
      for(auto i : v) sum+=i; 
    }
    template<typename T>
    column_printer & operator<<(const T & data)
    {
      if(index==0) os << begin_line;
      std::string c="s";
      if(typeid(T) == typeid(std::string)) c = 's';
      if(typeid(T) == typeid(double)) c = (float_format+'f');
      if(typeid(T) == typeid(int)) c = 'd';
      if(typeid(T) == typeid(unsigned)) c = 'd';
      os << boost::format("%"+align +boost::lexical_cast<std::string>(v[index])+c) %  data;
      if(index==(v.size()-1)) os << end_line << '\n';
      ++index %= v.size();
      return *this;
    }
    std::string operator()(void)
    {
      std::string s = os.str();
      os.clear();
      return s;
    }
    void set_align(const char * a)
    {
      align = a;
    }

    void set_float_format(const char * ff )
    {
     float_format = ff;
    }

    void print_line(char  delim)
    {
      os << begin_line;
      for(int i=0;i<sum;i++) os << delim;
      os << end_line << '\n';
    }

    void set_begin_line(std::string s) { begin_line = s; }
    void set_end_line(std::string s) { end_line = s; }

    void print_title(std::string title, char fill=' ')
    {
      size_t len = title.length();
      int begin = (sum-len)/2;
      int end = (sum+len)/2;
      os << begin_line;
      for(int i=0;i<begin;i++) os << fill;
      os << title;
      for(int i=end;i<sum;i++) os << fill;
      os << end_line;
      os<<'\n';
    }

  private:
    std::vector<int> v;
    unsigned index=0;
    boost::format fmt;
    std::ostringstream os;
    std::string float_format=".1";
    std::string align="";
    std::string begin_line="";
    std::string end_line="";
    int sum = 0;
};

inline std::ostream & operator<<(std::ostream & ostr, column_printer & col)
{
  ostr << col();
  return ostr;
}

