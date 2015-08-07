#cmake color echo function
#the final output color associated with terminal settings
#usage: cecho cred|cgreen|cyellow|cblue|cpurple|ccyan|cdef str_msg

function(cecho color output_msg)
    if(UNIX)
        set(TAG_RED     "\\033[31;1m")
        set(TAG_GREEN   "\\033[32;1m")
        set(TAG_YELLOW  "\\033[33;1m")
        set(TAG_BLUE    "\\033[34;1m")
        set(TAG_CPURPLE "\\033[35;1m")
        set(TAG_CYAN    "\\033[36;1m")
        set(TAG_RESET   "\\033[;0m\n")
        #comment for display ]

        if(color MATCHES cred)
            set(OMSG ${TAG_RED}${output_msg}${TAG_RESET})
            execute_process(COMMAND echo -ne ${OMSG})
        elseif(color MATCHES cgreen)
            set(OMSG ${TAG_GREEN}${output_msg}${TAG_RESET})
            execute_process(COMMAND echo -ne ${OMSG})
        elseif(color MATCHES cyellow)
            set(OMSG ${TAG_YELLOW}${output_msg}${TAG_RESET})
            execute_process(COMMAND echo -ne ${OMSG})
        elseif(color MATCHES cblue)
            set(OMSG ${TAG_BLUE}${output_msg}${TAG_RESET})
            execute_process(COMMAND echo -ne ${OMSG})
        elseif(color MATCHES cpurple)
            set(OMSG ${TAG_CPURPLE}${output_msg}${TAG_RESET})
            execute_process(COMMAND echo -ne ${OMSG})
        elseif(color MATCHES ccyan)
            set(OMSG ${TAG_CYAN}${output_msg}${TAG_RESET})
            execute_process(COMMAND echo -ne ${OMSG})
        else()
            message(STATUS ${output_msg})
        endif()
    else()
        #TODO
        message(STATUS ${output_msg})
    endif()
endfunction(cecho)

