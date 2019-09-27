# module: readobjfile
function (readobjfile rfile outputvariable keyword value)
    # read every line in the file as an item in a list and change .o to .cc
    file (STRINGS ${rfile} temp)
    string (REPLACE ".o" ".cc;" temp ${temp})
    
    # prepend directory path if "PREFIX" option is added
    if (${keyword} STREQUAL "PREFIX")
        set (dir ${value})
        set (temp2 " ")
        
        # prepend '$dir/' to every file in the list
        foreach (afile ${temp})
            string (APPEND temp2 "${dir}/${afile};")
        endforeach ()
        
        # set return value
        set (temp ${temp2})
        
        # clear temp variables
        unset (temp2)
        unset (dir)
    endif ()
    
    # remove the leading space from the list
    string (STRIP "${temp}" temp)
    
    # set value in calling scope of function
    set (${outputvariable} ${temp} PARENT_SCOPE)
endfunction ()

