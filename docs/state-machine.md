## 第十章：状态机

1. 状态机的概念
1. 定义一个状态机
1. 状态机的应用

		
## 状态机所要解决的问题

1. 计算机最擅长处理重复性的工作。
1. 现实世界中大量事物的工作流程取决于其他事物的状态。
1. 状态机就是针对多种状态之间的迁移而建立的一个简单数学模型。
1. 状态机可用来指导硬件模块或者软件模块的设计。

	
### 状态机的概念

1. 一个硬件或者一个软件模块；
1. 我们给状态机一个初始状态；
1. 然后给状态机一个事件；
1. 状态机根据事件和当前状态完成一个动作或者转换到另外一个状态；
1. 终止执行或者等待下一个事件。

如此，状态机就成了一个可重复工作的机器。

<img class="fragment" src="assets/figure-10.1.svg" height="400">

		
## 状态机相关术语

1. 术语
   - 状态（State）；可分为当前状态（现态）和迁移后的新状态（次态）。
   - 事件（Event）；又称为条件。
   - 动作（Action）。
   - 迁移、转换或变换（Transition）。
1. 分类
   - Moore 状态机：次态无关事件。
   - Mealy 状态机：次态和现态及事件均有关。

		
## 状态机描述方法

1. 状态迁移图
1. 状态迁移表
1. 文字描述

（以判断 C 语言整数立即数的进制为例）

	
### 状态迁移图

<img class="fragment" src="assets/figure-10.2.svg" height="400">

	
### 状态迁移表

| 现态     | 事件                                   | 动作                  |  次态    |
| -------- | -------                                | ---                   |  ---     |
| 未知     | 输入 0 字符                            | （无）                |  0前缀   |
| 未知     | 输入 1 ~ 9 字符                        | 返回十进制；终止      |  （无）  |
| 未知     | 输入其他字符                           | （无）                |  错误    |
| 0前缀    | 输入 x 字符                            | （无）                |  0x前缀  |
| 0前缀    | 输入 0 - 7 字符                        | 返回八进制；终止      |  （无）  |
| 0前缀    | 输入其他字符                           | （无）                |  错误    |
| 0x前缀   | 输入 0 到 9 或者 A/a 到 F/f 的字符     | 返回十六进制；终止    |  （无）  |
| 0x前缀   | 输入其他字符                           | （无）                |  错误    |
| 错误     | （无）                                 | 返回错误；终止        |  （无）  |

	
### 状态迁移描述

1. 状态：未知。
   - 若输入为 0，则进入“0前缀”状态；否则，
   - 若输入为 1 到 9 的字符，则执行返回十进制的动作；否则，
   - 进入“错误”态。
1. 状态：0前缀。
   - 若输入为 x 或者 X，则进入“0x前缀”状态；否则，
   - 若输入为 0 到 7 的字符，则执行返回八进制的动作，状态机终止；否则，
   - 进入“错误”状态。
1. 状态：0x前缀。
   - 若输入 0 到 9 或者 A/a 到 F/f 的字符，则执行返回十六进制的动作，状态机终止；否则，
   - 进入“错误”状态。
1. 状态：错误。
   - 执行返回错误的动作，状态机终止。

		
## 正确理解状态机

- 问题：是不是所有含有条件分支代码的程序或者函数都可以看成是一个状态机？

	
### 非状态机：无状态迁移

- 简单的事件驱动代码，并不构成状态机。

```c
#include <stdio.h>
#include <ctype.h>

int main(void)
{
    int c;

    while (1) {
        unsigned char uc;

        c = getchar();
        uc = (unsigned char)c;

        if (isdigit(c)) {
            int v = c - 48;
            printf("Got a digit character: %d\n", v);
        }
        else if (isupper(c)) {
            printf("Got a upper case letter: %c\n", c);
        }
        else if (islower(c)) {
            printf("Got a lower case letter: %c\n", c);
        }
        else if (c == EOF) {
            printf("Got EOF\n");
            break;
        }
        else if (uc >= 0x20 && uc < 0x80) {
            printf("Got a non-digit-and-alpha printable character: %c\n", c);
        }
        else if (uc < 0x20) {
            printf("Got a control character: 0x%x\n", c);
        }
        else {
            printf("Got a non-ASCII character: 0x%x\n", c);
        }
    }

    return 0;
}
```

	
### 状态机：必须存在状态迁移

比如我们要解析 [JSON](https://www.json.org/json-zh.html)：

```json
{
    "object": {},
    "array": [],
    "string": "This is a string",
    "number": 4567,
    "true": true,
    "false": false,
    "null": null
}
```

	
### JSON 的状态迁移图

<img class="fragment" src="assets/figure-10.3.svg" height="400">

		
## 状态机的应用

1. 解析器（parser）或分词器（tokenizer）

1. 使用状态迁移图/表理清迁移关系。
1. 状态数量较少时，一个包含上下文信息的函数搞定，内部使用条件分支代码。
1. 状态数量较多时，为每个状态定义回调函数，在回调函数中实现对这个状态的处理。

	
### 简单示例：判断 C 语言立即数的进制

1. 前缀 `0x` 表示十六进制、前缀 `0` 表示八进制，其他表示十进制。
1. 状态：未知、零前缀、八进制、十六进制、十进制、错误。

```c
enum {
    SSM_UNKNOWN = 0,
    SSM_PREFIX_ZERO,
    SSM_RESULT_FIRST,

    SSM_OCT = SSM_RESULT_FIRST,
    SSM_HEX,
    SSM_DEC,
    SSM_ERR,
};

struct _scale_state_machine {
    int state;
};

static int init_scale_state_machine(struct _scale_state_machine *sm)
{
    sm->state = SSM_UNKNOWN;
}

static int check_scale(struct _scale_state_machine *sm, char ch)
{
    switch (sm->state) {
    case SSM_UNKNOWN:
        if (ch == '0')
            sm->state = SSM_PREFIX_ZERO;
        else if (ch >= '1' && ch <= '9')
            sm->state = SSM_DEC;
        else
            sm->state = SSM_ERR;
        break;

    case SSM_PREFIX_ZERO:
        if (ch == 'x' || ch == 'X')
            sm->state = SSM_HEX;
        else if (ch >= '0' || ch <= '7')
            sm->state = SSM_OCT;
        else
            sm->state = SSM_ERR;
        break;
    }

    return sm->state;
}


int check_scale(const char *literal)
{
    struct _scale_state_machine sm;

    init_scale_state_machine(&sm);

    while (*literal) {
        int scale = check_scale(&sm, *literal);
        if (scale >= SSM_RESULT_FIRST)
            return scale;

        literal++;
    }

    return SSM_ERR;
}
```

	
### 复杂示例：HTML 解析器中的分词器

HTML 解析器包括两个部分：分词器 + DOM 树的构造

```html
<!DOCTYPE html>
<html>
    <head>
        <title>HTML DOM示例</title>
    </head>
    <body>
        <h1>一级标题</h1>
        <p>一个段落 &amp; 一个链接<a href="#">链接</a></p>
    </body>
</html>
```

	
<img class="fragment" src="assets/figure-10.4.svg" height="400">

	
[HTML 5.3 规范定义的分词器](https://html.spec.whatwg.org/#tokenization)一共有 80 个状态


> 13.2.5.1 Data（数据）状态
>
> 消耗下个输入字符：
> - U+0026 AMPERSAND (&)  
> 设置返回状态（return state）为 Data 状态，切换到 character reference（字符引用）状态。
> - U+003C LESS-THAN SIGN (<)  
> 切换到 tag open（标签打开）状态。
> - U+0000 NULL  
> 这是一个 unexpected-null-character （意外的空字符）解析错误；将当前输入字符激发（emit）为一个字符词元。
> - EOF  
> 激发 end-of-file （文件尾）词元。
> - 其他任意字符  
> 将当前输入字符激发为一个字符词元。

	
### [Lexbor 开源项目](https://github.com/lexbor/lexbor/)

- C 语言实现
- 不依赖于其他第三方库
- 易于理解

	
1) Lexbor 的 HTML 分词器数据结构

```c
struct lxb_html_tokenizer;
typedef struct lxb_html_tokenizer lxb_html_tokenizer;

/* 状态回调函数的原型 */
typedef const lxb_char_t *
(*lxb_html_tokenizer_state_f)(lxb_html_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end);

/* 词元回调函数的原型 */
typedef lxb_html_token_t *
(*lxb_html_tokenizer_token_f)(lxb_html_tokenizer_t *tkz,
                              lxb_html_token_t *token, void *ctx);

struct lxb_html_tokenizer {
    lxb_html_tokenizer_state_f       state;         // 现态对应的回调函数。
    lxb_html_tokenizer_state_f       state_return;  // 返回状态对应的回调函数。

    /* 获得有效词元后调用的回调函数及上下文。 */
    lxb_html_tokenizer_token_f       callback_token_done;
    void                             *callback_token_ctx;

    /* 用于记录当前待解析的文本位置等信息。 */
    lxb_char_t                       *start;
    lxb_char_t                       *pos;
    const lxb_char_t                 *end;
    const lxb_char_t                 *begin;
    const lxb_char_t                 *last;
    bool                             is_eof;

    /* 分词器状态，用于汇报错误码。 */
    lxb_status_t                     status;

    /* 篇幅所限，略去后续成员。 */
    ...
};
```

	
2) Lexbor 处理各个状态的代码

```c
/*
 * 12.2.5.1 Data state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_data(lxb_html_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end)
{
    lxb_html_tokenizer_state_begin_set(tkz, data);

    while (data != end) {
        switch (*data) {
            /* U+003C LESS-THAN SIGN (<) */
            case 0x3C:
                lxb_html_tokenizer_state_append_data_m(tkz, data);
                lxb_html_tokenizer_state_token_set_end(tkz, data);

                tkz->state = lxb_html_tokenizer_state_tag_open;
                return (data + 1);

            /* U+0026 AMPERSAND (&) */
            case 0x26:
                lxb_html_tokenizer_state_append_data_m(tkz, data + 1);

                tkz->state = lxb_html_tokenizer_state_char_ref;
                tkz->state_return = lxb_html_tokenizer_state_data;

                return data + 1;

            /* U+000D CARRIAGE RETURN (CR) */
            case 0x0D:
                if (++data >= end) {
                    lxb_html_tokenizer_state_append_data_m(tkz, data - 1);

                    tkz->state = lxb_html_tokenizer_state_cr;
                    tkz->state_return = lxb_html_tokenizer_state_data;

                    return data;
                }

                lxb_html_tokenizer_state_append_data_m(tkz, data);
                tkz->pos[-1] = 0x0A;

                lxb_html_tokenizer_state_begin_set(tkz, data + 1);

                if (*data != 0x0A) {
                    lxb_html_tokenizer_state_begin_set(tkz, data);
                    data--;
                }

                break;

            /*
             * U+0000 NULL
             * EOF
             */
            case 0x00:
                if (tkz->is_eof) {
                    /* Emit TEXT node if not empty */
                    if (tkz->token->begin != NULL) {
                        lxb_html_tokenizer_state_token_set_end_oef(tkz);
                    }

                    if (tkz->token->begin != tkz->token->end) {
                        tkz->token->tag_id = LXB_TAG__TEXT;

                        lxb_html_tokenizer_state_append_data_m(tkz, data);

                        lxb_html_tokenizer_state_set_text(tkz);
                        lxb_html_tokenizer_state_token_done_wo_check_m(tkz,end);
                    }

                    return end;
                }

                if (SIZE_MAX - tkz->token->null_count < 1) {
                    tkz->status = LXB_STATUS_ERROR_OVERFLOW;
                    return end;
                }

                tkz->token->null_count++;

                lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                             LXB_HTML_TOKENIZER_ERROR_UNNUCH);
                break;
        }

        data++;
    }

    lxb_html_tokenizer_state_append_data_m(tkz, data);

    return data;
}

/*
 * 12.2.5.6 Tag open state
 */
static const lxb_char_t *
lxb_html_tokenizer_state_tag_open(lxb_html_tokenizer_t *tkz,
                                  const lxb_char_t *data, const lxb_char_t *end)
{
    /* ASCII alpha */
    if (lexbor_str_res_alpha_character[ *data ] != LEXBOR_STR_RES_SLIP) {
        tkz->state = lxb_html_tokenizer_state_tag_name;

        lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, end);
        lxb_html_tokenizer_state_token_set_begin(tkz, data);

        return data;
    }

    /* U+002F SOLIDUS (/) */
    else if (*data == 0x2F) {
        tkz->state = lxb_html_tokenizer_state_end_tag_open;

        return (data + 1);
    }

    /* U+0021 EXCLAMATION MARK (!) */
    else if (*data == 0x21) {
        tkz->state = lxb_html_tokenizer_state_markup_declaration_open;

        lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, end);

        return (data + 1);
    }

    /* U+003F QUESTION MARK (?) */
    else if (*data == 0x3F) {
        tkz->state = lxb_html_tokenizer_state_bogus_comment_before;

        lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, end);
        lxb_html_tokenizer_state_token_set_begin(tkz, data);

        lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                     LXB_HTML_TOKENIZER_ERROR_UNQUMAINOFTANA);

        return data;
    }

    /* EOF */
    else if (*data == 0x00) {
        if (tkz->is_eof) {
            lxb_html_tokenizer_state_append_m(tkz, "<", 1);

            lxb_html_tokenizer_state_token_set_end_oef(tkz);
            lxb_html_tokenizer_state_token_emit_text_not_empty_m(tkz, end);

            lxb_html_tokenizer_error_add(tkz->parse_errors, tkz->token->end,
                                         LXB_HTML_TOKENIZER_ERROR_EOBETANA);

            return end;
        }
    }

    lxb_html_tokenizer_state_append_m(tkz, "<", 1);

    lxb_html_tokenizer_error_add(tkz->parse_errors, data,
                                 LXB_HTML_TOKENIZER_ERROR_INFICHOFTANA);

    tkz->state = lxb_html_tokenizer_state_data;

    return data;
}
```

	
3) 使用 Lexbor 分词器

```c
lxb_status_t html_tokenizer_chunk(const lxb_char_t *data, size_t size)
{
    lxb_html_tokenizer_t *tkz;

    /* 创建一个分词器 */
    tkz = lxb_html_tokenizer_create();

    /* 初始化分词器；该函数会将初始状态设置为 Data 状态。 */
    lxb_html_tokenizer_init(tkz);

    const lxb_char_t *end = data + size;

    tkz->is_eof = false;
    tkz->status = LXB_STATUS_OK;
    tkz->last = end;

    /* 喂数据给状态机。*/
    while (data < end) {
        /* 调用当前状态的回调函数。 */
        data = tkz->state(tkz, data, end);
    }

    return tkz->status;
}
```

		
## 作业

1. 编写一个 C 程序的分词器，构造一个粗略的抽象语法树，并按照指定的编码风格格式化源程序。

