## 第九讲：状态机

- 时间：2022 年 1 月 6 日 17:30
- 地点：微信视频号“考鼎录”直播间
- [预约或回放](#/grand-finale)

		
## 提纲

1. 状态机所要解决的问题
1. 状态机相关理论
1. 正确理解状态机
1. 状态机的软件实现

		
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

		
## 状态机相关理论

1. 术语
   - 状态（State）；可分为当前状态（现态）和迁移后的新状态（次态）。
   - 事件（Event）；又称为条件。
   - 动作（Action）。
   - 迁移、转换或变换（Transition）。
1. 分类
   - Moore 状态机：次态无关事件。
   - Mealy 状态机：次态和现态及事件均有关。

	
### 方法

1. 状态迁移图
1. 状态迁移表

		
## 正确理解状态机

- 问题：是不是所有含有条件分支代码的程序或者函数都可以看成是一个状态机？

	
### 非状态机：无状态跃迁

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

		
## 状态机的软件实现

1. 确定性状态机
   1. 有限的状态
   1. 有限的输入事件
   1. 有确定的状态迁移规则
1. 自定义状态机
   1. 自定义状态
   1. 有限的输入事件
   1. 某种确定的状态迁移规则

		
## 确定性状态机

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

1. [HTML 规范](https://html.spec.whatwg.org/#tokenization)：一共 80 个状态
1. [Lexbor 的实现](https://github.com/lexbor/lexbor/)

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

		
## 自定义状态机

1. 事件是确定的、有限的。
1. 状态是一种抽象对象而不是一个简单的枚举变量；状态被组织为链表或者树形数据结构。
1. 每个状态可根据输入的事件构造一个抽象的迁移对象。
1. 迁移对象实现动作及状态迁移；通常，状态迁移发生在相邻的状态节点之间。
1. 状态机构造状态数据结构，并根据状态返回的迁移函数运行，直到停止。

	
### 应用场景

1. 轨迹生成器：
   - 状态是代表不同轨迹的时间曲线方程（如线性、贝塞尔曲线等）。
   - 事件是定时器。
1. 动画控制器：
   - 状态是不同的动画效果（如放大、缩小、淡入、淡出、渐变）。
   - 事件是定时器或者用户输入。
1. 参考源代码：
   - [头文件](https://github.com/VincentWei/cell-phone-ux-demo/blob/master/include/StateMachine.hh)
   - [源文件](https://github.com/VincentWei/cell-phone-ux-demo/blob/master/common/StateMachine.cc)

		
## 下一讲预告

- 主题：为性能编码
- 地点：微信视频号“考鼎录”直播间
- 时间：待定

		
## Q & A

