-- There's no subdirs currently, so only list all files
local base = ...
local files = list_dir(base)
local articles = { base .. "index.md" }
local tailpat ="(([^/]+).lagda.md)$"
for _, f in pairs(files) do
    if f.type == "file" and f.path:match(".lagda.md$") then
        local target = f.path:gsub(tailpat, "%2.output.md")
        articles[#articles + 1] = target
        if need_rebuild(f.path, target) then
            os.execute("cd " .. base ..
                " && agda --html-dir=inter --html --html-highlight=code " ..
                f.path:match("([^/]+.lagda.md)$"))
            os.rename(f.path:gsub(tailpat, "inter/%2.md"), target)
        end
    end
end
return { articles = articles, build_args = "--lua-filter=" .. base .. "/preproc.lua " .. default_build_args }
