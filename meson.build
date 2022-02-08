project(
  'bsh',
  'c',
  version : '1.0',
  default_options: [
    'c_std=c99',
    'warning_level=2',
    'werror=true',
  ],
)

# add_project_arguments('-fsanitize=address', language : 'c')
# add_project_link_arguments('-fsanitize=address', language : 'c')

cflags = ['-D_GNU_SOURCE', '-D_POSIX_C_SOURCE=200809L']
add_project_arguments(cflags, language: 'c')

incdirs = [
  include_directories('./src/lexer'),
  include_directories('./src/parser'),
  include_directories('./src/shell_input'),
  include_directories('./src/ast_evaluation'),
  include_directories('./src/builtins'),
  include_directories('./src/variables'),
  include_directories('./src/loops'),
  include_directories('./src/functions'),
  include_directories('./src/'),
]

common = [
  'src/lexer/lexer.c',
  'src/lexer/lexer_print.c',
  'src/lexer/ionumbers.c',
  'src/lexer/create_token.c',
  'src/lexer/keywords.c',
  'src/lexer/split_input.c',
  'src/lexer/spaces.c',
  'src/lexer/export.c',
  'src/lexer/alias.c',
  'src/parser/parser.c',
  'src/parser/parser_high_level.c',
  'src/parser/parser_conditions.c',
  'src/parser/parser_commands.c',
  'src/parser/parser_loops.c',
  'src/parser/ast.c',
  'src/ast_evaluation/pretty_print.c',
  'src/ast_evaluation/ast_evaluation.c',
  'src/ast_evaluation/builtin_exec.c',
  'src/ast_evaluation/inner_exec.c',
  'src/ast_evaluation/prog_exec.c',
  'src/ast_evaluation/pipe.c',
  'src/ast_evaluation/redir.c',
  'src/ast_evaluation/redir_tools.c',
  'src/shell_input/shell_input.c',
  'src/builtins/builtins.c',
  'src/variables/var_list.c',
  'src/builtins/echo.c',
  'src/builtins/cd.c',
  'src/builtins/continue.c',
  'src/builtins/break.c',
  'src/builtins/dot.c',
  'src/parser/parse_functions.c',
  'src/loops/loop_stack.c',
  'src/builtins/exit.c',
  'src/builtins/export.c',
  'src/functions/functions.c',
  'src/builtins/unset.c',
]

token_printer = executable(
    # name of the output executable
    'bsh',
    # source files for the executable
    common + [ 'src/bsh.c' ],
    # these are passed as -I
    include_directories: incdirs
)
