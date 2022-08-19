vim9script
def OpenBuf()
    if g:asyncrun_code != 0
        return
    end
    if !exists("g:server_bufnr") || !g:server_bufnr
        g:server_bufnr = term_start(["python", "-m", "http.server"], {
            exit_cb: (..._) => {
                silent! exec "bw! " .. g:server_bufnr
                g:server_bufnr = 0
            },
            hidden: true,
        })
    endif
    exec "!xdg-open http://localhost:8000/" .. fnamemodify(expand("%"), ":s/^src//:r") .. ".html"
enddef
def InitBuf()
    command! -bar -buffer Compile w | exec 'AsyncRun ./build.sh -i'
    command! -bar -buffer Run Compile | au User AsyncRunStop ++once OpenBuf()
enddef
augroup vimrc
    au BufRead *.md InitBuf()
augroup END
