
all:
	clang -Wall -Wextra -Wpedantic -Werror -lpthread lock-maybe.c -o lock-maybe
	clang -Wall -Wextra -Wpedantic -Werror -lpthread spin-lock.c -o spin-lock
	clang -Wall -Wextra -Wpedantic -Werror -lpthread atomic-lock.c -o atomic-lock
	clang -Wall -Wextra -Wpedantic -Werror -lpthread lock-really.c -o lock-really
	clang -Wall -Wextra -Wpedantic -Werror -lpthread mutex-lock.c -o mutex-lock
	clang -Wall -Wextra -Wpedantic -Werror -lpthread cond_var.c -o cond_var

clean:
	rm -f lock-maybe spin-lock atomic-lock lock-really mutex-lock cond_var
