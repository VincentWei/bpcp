## 第七讲：子驱动程序实现模型

1. 抽象的重要性
1. 子驱动程序实现模型的大量应用
1. 正确区分机制和策略
1. 子驱动程序实现模型的演进

		
## 设计和编码水平弱的根本原因

抽象能力不足

1. 对事物的正确认知建立在归纳总结之上
1. 抽象是归纳总结的一种升华
1. 如何提高自己的抽象能力：学好高等数学
   - 举例：群论
   - 举例：集合的可数性

		
## 子驱动程序模式在稍有规模的 C 项目中大量应用

1. Unix 的万物皆文件
1. Unix/Linux 内核的虚拟文件系统以及设备驱动程序
1. MiniGUI 中支持多种类型的图片格式以及逻辑字体

		
## 子驱动程序模式的一般实现套路

1. 一套聚类接口
1. 一些公共数据组成的抽象对象（数据结构）
1. 一组函数指针组成的操作集（数据结构）
1. 针对不同子类的操作集实现

	
### 子驱动程序实现模型中的各个构成部分

![Figure 8.1](figure-8.1.svg)

		
## STDIO 接口的实现

```c
#include <stdio.h>

FILE *fopen(const char *pathname, const char *mode);
FILE *fmemopen(void *buf, size_t size, const char *mode);

int fclose(FILE *stream);
```

	
### 对象和操作集

```c
struct _file_obj;
typedef struct _file_obj file_obj;

struct _file_ops {
    /* no need
    file_obj *open(void *pathname_buf, size_t size, const char *mode);
    */
    ssize_t read(file_obj *file, void *buf, size_t count);
    ssize_t write(file_obj *file, const void *buf, size_t count);
    off_t lseek(file_obj *file, off_t offset, int whence);
    void close(file_obj *file);
};

struct _FILE;
typedef struct _FILE FILE;
```

	
### `FILE` 结构：基本设计

```c
struct _FILE {
    struct _file_ops*   ops;
    struct _file_obj*   obj;
};

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    if (size == 0 || nmemb == 0)
        return 0;

    ssize_t n = stream->ops->read(stream->obj, ptr, size * nmemb);

    if (n <= 0)
        return 0;

    return n/nmemb;
}

```

	
### `fopen` 的实现

```c
struct _file_obj {
    int fd;
};

static ssize_t file_read(file_obj *file, void *buf, size_t count)
{
    return read(file->fd, buf, count);
}

static struct _file_ops file_file_ops = {
    .read: file_read;
    ...
};

FILE *fopen(const char *pathname, const char *std_mode)
{
    int fd;
    int flags;
    mod_t mode;

    /* evaluate flags and mode accoding to std_mode */
    ...

    FILE *file = NULL;

    /* call open, or creat */
    fd = open(pathname, flags, mode);
    if (fd >= 0) {
        file_obj *obj = malloc(sizeof(file_obj));
        if (obj) {
            obj.fd = fd;
            file = calloc(1, sizeof(FILE));
            if (file) {
                file.obj = obj;
                file.ops = &file_file_ops;
            }
            else {
                free (obj);
            }
        }
    }

    return file;
}

```

	
### `fmemopen` 的实现

```c
#define MEM_FILE_FLAG_READABLE      0x01
#define MEM_FILE_FLAG_WRITABLE      0x02

struct _file_obj {
    void *          buf;
    size_t          size;

    unsigned int    flags;
    off_t           pos;
};

static ssize_t mem_read(file_obj *file, void *buf, size_t count)
{
    if (file->pos + count <= file->size) {
        memcpy (buf, file->buf + file->pos, count);
        file->pos += count;
        return count;
    }
    else {
        ssize_t n = file->size - file->pos;

        if (n > 0) {
            memcpy (buf, file->buf + file->pos, n);
            file->pos += n;
            return n;
        }

        return 0;
    }

    return -1;
}

static struct _file_ops mem_file_ops = {
    .read: mem_read;
    ...
};

FILE *fmemopen(void *buf, size_t size, const char *std_mode)
{
    unsigned int flags;
    /* evaluate flags according to std_mode */
    ...

    FILE *file = NULL;

    file_obj *obj = malloc(sizeof(file_obj));
    if (obj) {
        obj->buf = buf;
        obj->size = size;
        obj->flags = flags;
        obj->pos = 0;

        file = calloc(1, sizeof(FILE));
        if (file) {
            file.obj = obj;
            file.ops = &mem_file_ops;
        }
        else {
            free (obj);
        }
    }

    return file;
}
```

	
### 进一步考虑

1. STDIO 是带有缓冲区功能的，缓冲区信息应该在 `FILE` 中维护还是在 `file_obj` 中维护？
1. 当前读写位置在什么地方维护？
1. 子驱动程序设计的关键点
   1. 抽象对象的数据结构如何确定？
   1. 操作集如何取舍？

		
## 正确区分机制和策略

- 机制：需要提供什么功能
- 策略：如何使用这些功能
- 以 STDIO 实现为例：
  1. 带有缓冲区支持的格式化输入输出属于使用策略，对不同类型的文件对象是一样的。
  1. 文件操作集提供的就是机制，符合最小的完备集合原则。

		
## MiniGUI 中的逻辑字体

- 现代图形系统中文字的显示过程：
   1. 字符串解码取出一个个字符。
   1. 如果包含有复杂书写（如阿拉伯、希伯来、印度语系文字），需要做双向排版处理。
   1. 字符到字型：如果含有复杂书写文字，需要做字符成型（shaping）处理，比如组合字符问题。
   1. 从字体文件中获取字型信息（点阵或矢量）。
   1. 交由图形系统渲染字型到指定的缓冲区位置。

	
### 主要接口

```c
/* Create a logical font by a font name specified by
 * \a font_name. Note that since version 4.0.0, you can specify up
 * to 7 family names in the LOGFONT name, such as:
 *
 *      ttf-Courier,宋体,Naskh,SansSerif-rrncns-U-16-UTF-8
 */
PLOGFONT CreateLogFontByName(const char* font_name);
void GUIAPI DestroyLogFont (PLOGFONT log_font);

PLOGFONT GUIAPI SelectFont (HDC hdc, PLOGFONT log_font);
int GUIAPI TextOutLen (HDC hdc, int x, int y, const char* spText, int len);

Achar32 GUIAPI GetACharValue (LOGFONT* logfont, const char* mchar,
        int mchar_len, const char* pre_mchar, int pre_len);

Uchar32 GUIAPI AChar2UChar(LOGFONT* logfont, Achar32 chv);

int GUIAPI GetGlyphsExtentFromUChars(LOGFONT* logfont_upright,
        const Achar32* uchars, int nr_uchars, const BreakOppo* break_oppos,
        Uint32 render_flags, int x, int y,
        int letter_spacing, int word_spacing, int tab_size, int max_extent,
        SIZE* line_size, Glyph32* glyphs, GLYPHEXTINFO* glyph_ext_info,
        GLYPHPOS* glyph_pos, LOGFONT** logfont_sideways);

int GUIAPI DrawGlyphStringEx(HDC hdc,
        LOGFONT* logfont_upright, LOGFONT* logfont_sideways,
        const Glyph32* glyphs, const GLYPHPOS* glyph_pos,
        int nr_glyphs);
```

	
### `LOGFONT`

```c
struct _DEVFONT;
typedef struct _DEVFONT DEVFONT;

/**
  * The logical font structure.
  * \note All fields are read-only.
  */
typedef struct _LOGFONT {
    /** The type of the logical font. */
    char* type;
    /** The family name of the logical font. */
    char* family;
    /** The charset of the logical font. */
    char* charset;
    /** The styles of the logical font. */
    DWORD32 style;
    /** The size of the logical font. */
    int  size;
    /** The rotation angle of the logical font. */
    int  rotation;
    /** The ascent of the logical font. */
    int  ascent;
    /** The descent of the logical font. */
    int  descent;
    /** The size requested initially. */
    int  size_request;

    /*
     * The following fields are internally used.
     * They may be changed in the future.
     */

    // The scale factors of devfonts
    unsigned short  scales[MAXNR_DEVFONTS];
    // The devfonts for the logfont
    DEVFONT*        devfonts[MAXNR_DEVFONTS];
} LOGFONT;
```

	
### `DEVFONT`

```c
/**
 * The device font structure.
 * \note All fields are read-only.
 */
struct _DEVFONT {
    /**
      * The device font name.
      * The family name supports aliases since 4.0.0:
      *
      *     <fonttype>-<family[,aliase]*>-<styles>-<width>-<height>-<charset[,charset]*>
      *
      * for example:
      *
      *     ttf-courier,monospace-rrncnn-0-0-ISO8859-1,UTF-8
      */
    char             name [LEN_UNIDEVFONT_NAME + 1];

    /** The styles of the device font. */
    DWORD32          style;

    /*
     * The following fields are internally used.
     * They may changed in the future.
     */
    // indicating if the data need to be unloaded before delete a devfont
    BOOL             need_unload;

    // The pointer to font operation structure.
    FONTOPS*         font_ops;

    // The pointer to character set operation structure.
    CHARSETOPS*      charset_ops;

    // The pointer to next device font.
    struct _DEVFONT* next;

    // The device font used data.
    void*            data;

    // The device font used relationship.
    void*            relationship;
};
```

	
### `CHARSETOPS`

```c
/** The character set operation structure. */
struct _CHARSETOPS
{
    /** The character number of the character set. */
    int nr_chars;

    /** The byte number of the max length character. */
    int bytes_maxlen_char;

    /** The name of the character set. */
    const char* name;

    /** Default character. */
    Achar32 def_char_value;

    /** The method to get the length of the first character. */
    int (*len_first_char) (const unsigned char* mstr, int mstrlen);

    /** The method to get the character index. */
    Achar32 (*get_char_value) (const unsigned char* pre_mchar, int pre_len,
            const unsigned char* cur_mchar, int cur_len);

    /** The method to get the require shape type of a mchar. */
    Achar32 (*get_shaped_char_value) (const unsigned char* cur_mchar, int cur_len,
            int shape_type);

    /** The method to get the type of one character. */
    unsigned int (*char_type) (Achar32 chv);

    /** The method to get character number in the string function. */
    int (*nr_chars_in_str) (const unsigned char* mstr, int mstrlen);

    /** The method to judge a given charset name is belong to the character set */
    int (*is_this_charset) (const unsigned char* charset);

    /** The method to get  the length of the first substring. */
    int (*len_first_substr) (const unsigned char* mstr, int mstrlen);

    /** The method to get next word in the specitied length string function. */
    const unsigned char* (*get_next_word) (const unsigned char* mstr,
                int strlen, WORDINFO* word_info);

    /** The method to get the position of the first character in the
     *  specified length string function.
     */
    int (*pos_first_char) (const unsigned char* mstr, int mstrlen);

    /** The method to get the BIDI type of one character. */
    BidiType (*bidi_char_type) (Achar32 chv);

    /** Get mirrored character */
    BOOL (*bidi_mirror_char) (Achar32 chv, Achar32* mirrored);

#ifdef _MGCHARSET_UNICODE
    /** The method to convert a mchar32 value to 32 bit UNICODE function. */
    Uchar32 (*conv_to_uc32) (Achar32 chv);

    /** The method to convert \a wc to multily byte character function. */
    int (*conv_from_uc32) (Uchar32 wc, unsigned char* mchar);
#endif /* _UNICODE_SUPPORT */
};
```

	
### `FONTOPS`

```c
struct _FONTOPS
{
    /** The method to get the glyph bitmap type . */
    DWORD (*get_glyph_bmptype) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get average width function. */
    int (*get_ave_width) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get max width function. */
    int (*get_max_width) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get font height function. */
    int (*get_font_height) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get font size function. */
    int (*get_font_size) (LOGFONT* logfont, DEVFONT* devfont, int expect, int slot);

    /** The method to get font ascent function. */
    int (*get_font_ascent) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get font descent function. */
    int (*get_font_descent) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to judge the glyph is exist function. */
    BOOL (*is_glyph_existed) (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value);

    /** The method to get character advance function. */
    int (*get_glyph_advance) (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 galph_value, int* px, int* py);

    /** The method to get character bound box function. */
    int (*get_glyph_bbox) (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 galph_value, int* px, int* py, int* pwidth, int* pheight);

    /** The method to get mono-bitmap function. */
    const void* (*get_glyph_monobitmap) (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value, SIZE* sz, int* pitch, unsigned short* scale);

    /** The method to get grey bitmap, pitch and scale function */
    const void* (*get_glyph_greybitmap) (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 galph_value, SIZE* sz, int* pitch, unsigned short* scale);

    /** The method to get the pre-rendered bitmap of one glyph. */
    int (*get_glyph_prbitmap) (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value, BITMAP *bmp);

    /* The method to preproccess start string output fuction, call this
     * function before output a string, can be NULL.
     */
    void (*start_str_output) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to instance new device font function, can be NULL. */
    DEVFONT* (*new_instance) (LOGFONT* logfont, DEVFONT* devfont,
            BOOL need_sbc_font);

    /** The method to delete instance of device font function, can be NULL. */
    void (*delete_instance) (DEVFONT* devfont);

    int (*is_rotatable)  (LOGFONT* logfont, DEVFONT* devfont, int rot_desired);

    /** The method to load data of a devfont from font file
     * FIXME real_fontname: return real name in file by a char point
     */
    void* (*load_font_data) (DEVFONT* devfont, const char* fontname, const char* filename);

    /** The method to unload data of a devfont*/
    void (*unload_font_data) (DEVFONT* devfont, void* data);

    /**
     * The method to get the glyph value according to the character value.
     * If it is NULL, the glyph value will be equal to the character value.
     *
     * Since 4.0.0.
     */
    Glyph32 (*get_glyph_value) (LOGFONT* logfont, DEVFONT* devfont, Achar32 chv);

    /**
     * The method to get the kerning delta values
     *
     * Since 4.0.0.
     */
    void (*get_kerning) (LOGFONT* logfont, DEVFONT* devfont,
        Glyph32 prev, Glyph32 curr, int* delta_x, int* delta_y);

    /**
     * The method to get the FreeType2 FT_Face objece; only valid for FreeType2
     * font engine
     *
     * Since 4.0.0.
     */
    void* (*get_ft_face) (LOGFONT* logfont, DEVFONT* devfont);
};
```

		
## 作业

