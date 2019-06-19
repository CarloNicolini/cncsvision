if(WIN32)
    add_subdirectory(expChiara)
    add_subdirectory(expRobertGrasping)
    if(GLEW_FOUND)
        add_subdirectory(expKatja)
    endif(GLEW_FOUND)
    add_subdirectory(HVdiscrimination)
    add_subdirectory(expCampagnoli)
    add_subdirectory(expDhanraj)
    add_subdirectory(expEvan)
    add_subdirectory(expKarl)
endif(WIN32)


