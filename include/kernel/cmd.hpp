#pragma once

void runcmd(const char*, int);

void cmd_help();
void cmd_hello();
void cmd_hwinfo();
void cmd_reboot();

using cmd_fp = void (*)();

struct Cmd {
  const char* _name;
  const char* _help;
  cmd_fp _fp;
  inline const char* name() const;
  inline const char* help() const;
  inline cmd_fp fp() const;
};

extern const Cmd cmds[];
extern const int ncmd;

// some hack to reloc address
template <typename T>
T REL(T addr) {
  static int base_idx = 0;
  return (T)((char*)addr - (char*)cmds[base_idx]._fp + (char*)&cmd_help);
}

inline const char* Cmd::name() const {
  return REL(this->_name);
}
inline const char* Cmd::help() const {
  return REL(this->_help);
}
inline cmd_fp Cmd::fp() const {
  return REL(this->_fp);
}
