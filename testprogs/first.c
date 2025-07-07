#define FOO (*(unsigned int *)128)
#define BAR (*(unsigned int *)132)

void myfunc() {
  FOO = 5;
  BAR = 10;
  FOO += BAR;
}
