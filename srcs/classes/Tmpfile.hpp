#ifndef TMP_FILE_HPP
# define TMP_FILE_HPP

# include <iostream>
# include <string>
# include <unistd.h>

class Tmpfile {
    protected:
        static char 				*_path;
        static std::string          _nextnameprefix;
        static void                 _update_nextnameprefix(void);
        static std::string          _get_next_name(void);
        static bool                 _does_nextfile_exist(void);
        std::string                 _filename;
        int                         _fd;

    public:
        Tmpfile(void);
        Tmpfile(Tmpfile const &src);
        virtual ~Tmpfile(void);

        Tmpfile           &operator=(Tmpfile const &rhs);
        int const           &get_fd(void) const;
        std::string const   &get_filename(void) const;
        bool                is_read_ready(void) const;
        bool                is_write_ready(void) const;
        size_t              get_size(void) const;
        void                reset_cursor(void);
};

#endif
