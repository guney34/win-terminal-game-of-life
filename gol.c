#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <windows.h>

#define INTERVAL_MS 50

int main(void)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	printf("Resize the terminal and then press Enter to continue: ");
	getchar();
	system("cls");

	CONSOLE_SCREEN_BUFFER_INFO Buffer;
	GetConsoleScreenBufferInfo(hStdout, &Buffer);

	int Width = Buffer.dwSize.X - 1;
	int Height = Buffer.dwSize.Y - 1;

	int *A = (int *)malloc(Width * Height * sizeof(int));
	int *B = (int *)malloc(Width * Height * sizeof(int));

	CONSOLE_CURSOR_INFO Cursor;
	Cursor.dwSize = 1;
	Cursor.bVisible = FALSE;
	SetConsoleCursorInfo(hStdout, &Cursor);

	_setmode(_fileno(stdout), _O_U16TEXT);

	srand((unsigned int)time(0));

	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			A[x*Height + y] = rand() % 2;
		}
	}

	for (int y = 0; y < Height; ++y)
	{
		for (int x = 0; x < Width; ++x)
		{
			if (A[x*Height + y]) putwchar(L'\u2588');
			else putwchar(L'\u00B7');
		}
		putwchar(L'\n');
	}

	memcpy(B, A, Width * Height * sizeof(int));

	while (1)
	{
		for (int y = 0; y < Height; ++y)
		{
			for (int x = 0; x < Width; ++x)
			{
				if (A[x*Height + y] != B[x*Height + y])
				{
					SetConsoleCursorPosition(hStdout, (COORD) { (SHORT)x, (SHORT)y });
					if (B[x*Height + y]) putwchar(L'\u2588');
					else putwchar(L'\u00B7');
				}
			}
		}

		memcpy(A, B, Width * Height * sizeof(int));

		for (int y = 0; y < Height; ++y)
		{
			for (int x = 0; x < Width; ++x)
			{
				int neighbours = 0;

				for (int rel_y = -1; rel_y <= 1; ++rel_y)
				{
					for (int rel_x = -1; rel_x <= 1; ++rel_x)
					{
						if (x + rel_x < 0 || y + rel_y < 0 ||
							x + rel_x >= Width || y + rel_y >= Height ||
							(rel_x == 0 && rel_y == 0))
						{
							continue;
						}

						if (A[(x + rel_x)*Height + (y + rel_y)]) ++neighbours;
					}
				}

				if (A[x*Height + y])
				{
					if (neighbours < 2 || neighbours > 3) B[x*Height + y] = 0;
					else B[x*Height + y] = 1;
				}
				else
				{
					if (neighbours == 3) B[x*Height + y] = 1;
					else B[x*Height + y] = 0;
				}
			}
		}
		Sleep(INTERVAL_MS);
	}
}