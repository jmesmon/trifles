#include <stdio.h>
#include "share.h"

extern void other_mess(void);
int main(void)
{
	s_mixed.x = 1;
	s_mixed.y = 2;

	s_simple = 4;
	s_separate.x = 5;
	s_separate.y = 6;

	other_mess();

	printf("%d %d %d\n", s_mixed.x, s_separate.x, s_mixed.y);
	return 0;
}
