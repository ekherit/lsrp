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
    print_predicate(long long event, const std::vector<int> && denom) 
    {
      for(size_t i=0; i<denom.size();i++)
      {
        isprint  = isprint || ( (event < denom[i+1] || i==(denom.size()-1) ) && (event % denom[i] == 0) );
      }
    }
    bool operator()(void) const { return isprint; }
    explicit operator bool() const noexcept { return isprint; }
  private:
    bool isprint=false;
};
