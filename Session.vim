let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/Projects/C/allc
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +33 main.c
badd +2 man://memset(3)
badd +0 term://~/Projects/C/allc//92865:/bin/bash
badd +214 src/strbuf.h
badd +5 examples/cstr.c
badd +24 src/cstr.h
badd +1 strbuf
badd +1 src
argglobal
%argdel
$argadd main.c
edit main.c
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
wincmd _ | wincmd |
split
1wincmd k
wincmd w
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 85 + 128) / 256)
exe '2resize ' . ((&lines * 34 + 35) / 71)
exe 'vert 2resize ' . ((&columns * 170 + 128) / 256)
exe '3resize ' . ((&lines * 34 + 35) / 71)
exe 'vert 3resize ' . ((&columns * 84 + 128) / 256)
exe '4resize ' . ((&lines * 34 + 35) / 71)
exe 'vert 4resize ' . ((&columns * 85 + 128) / 256)
argglobal
if bufexists(fnamemodify("term://~/Projects/C/allc//92865:/bin/bash", ":p")) | buffer term://~/Projects/C/allc//92865:/bin/bash | else | edit term://~/Projects/C/allc//92865:/bin/bash | endif
if &buftype ==# 'terminal'
  silent file term://~/Projects/C/allc//92865:/bin/bash
endif
balt main.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=10
setlocal fml=1
setlocal fdn=3
setlocal fen
let s:l = 39 - ((2 * winheight(0) + 34) / 69)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 39
normal! 034|
wincmd w
argglobal
balt src/cstr.h
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=10
setlocal fml=1
setlocal fdn=3
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 33 - ((28 * winheight(0) + 17) / 34)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 33
normal! 024|
wincmd w
argglobal
if bufexists(fnamemodify("src/strbuf.h", ":p")) | buffer src/strbuf.h | else | edit src/strbuf.h | endif
if &buftype ==# 'terminal'
  silent file src/strbuf.h
endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=10
setlocal fml=1
setlocal fdn=3
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 145 - ((4 * winheight(0) + 17) / 34)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 145
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("src/cstr.h", ":p")) | buffer src/cstr.h | else | edit src/cstr.h | endif
if &buftype ==# 'terminal'
  silent file src/cstr.h
endif
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=10
setlocal fml=1
setlocal fdn=3
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 32 - ((24 * winheight(0) + 17) / 34)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 32
normal! 011|
wincmd w
2wincmd w
exe 'vert 1resize ' . ((&columns * 85 + 128) / 256)
exe '2resize ' . ((&lines * 34 + 35) / 71)
exe 'vert 2resize ' . ((&columns * 170 + 128) / 256)
exe '3resize ' . ((&lines * 34 + 35) / 71)
exe 'vert 3resize ' . ((&columns * 84 + 128) / 256)
exe '4resize ' . ((&lines * 34 + 35) / 71)
exe 'vert 4resize ' . ((&columns * 85 + 128) / 256)
tabnext 1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
