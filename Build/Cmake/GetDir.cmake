#cmake find directories 
#usage: cecho cred|cgreen|cyellow|cblue|cpurple|ccyan|cdef str_msg

macro(cget_dir  ret_dirs  parentdir)
    set(EXEC_CMD "find ${parentdir} -type d | grep -v .svn")
    exec_program(${EXEC_CMD} AGRS OUTPUT_VARIABLE ${ret_dirs})
endmacro(cget_dir)
