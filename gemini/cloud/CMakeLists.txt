add_subdirectory(dropbox)
if(TARGET LibGit2::LibGit2)
    add_subdirectory(git)
endif()
