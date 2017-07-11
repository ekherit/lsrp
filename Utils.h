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

void print_memory_usage(long run = 0)
{
  std::ofstream mem_usage("mem_usage.txt",std::ios_base::app);
  mem_usage << "MEMORY USAGE: " << run << ": " << GetPhysMemUsage() << " kB" << std::endl;
}
