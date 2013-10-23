
valgrind --tool=memcheck --leak-check=full --track-origins=yes --log-file=valgrind.log $(which ./gui_app)
