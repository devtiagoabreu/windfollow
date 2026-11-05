param(
    [string]$WebDir = (Join-Path $PSScriptRoot "..\firmware\windfollow\web"),
    [string]$OutFile = (Join-Path $PSScriptRoot "..\firmware\windfollow\WebAssets.h")
)

$files = @(
    @{ Var = "WIND_FOLLOW_INDEX_HTML"; Path = (Join-Path $WebDir "index.html") },
    @{ Var = "WIND_FOLLOW_STYLE_CSS";  Path = (Join-Path $WebDir "css\style.css") },
    @{ Var = "WIND_FOLLOW_APP_JS";     Path = (Join-Path $WebDir "js\app.js") }
)

$sb = New-Object System.Text.StringBuilder
[void]$sb.AppendLine("#ifndef WIND_FOLLOW_WEB_ASSETS_H")
[void]$sb.AppendLine("#define WIND_FOLLOW_WEB_ASSETS_H")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("// Arquivo gerado por tools/generate_web_assets.ps1 - nao edite manualmente.")
[void]$sb.AppendLine("// Fonte: firmware/windfollow/web/{index.html, css/style.css, js/app.js}")
[void]$sb.AppendLine("")

foreach ($f in $files) {
    if (-not (Test-Path -LiteralPath $f.Path)) {
        throw "Arquivo nao encontrado: $($f.Path)"
    }
    $content = [System.IO.File]::ReadAllText($f.Path, [System.Text.Encoding]::UTF8)
    $content = $content.Replace(')WFWEB"', ')WFWE"')

    [void]$sb.AppendLine('static const char ' + $f.Var + '[] PROGMEM = R"WFWEB(')
    [void]$sb.Append($content)
    if (-not $content.EndsWith("`n")) { [void]$sb.AppendLine("") }
    [void]$sb.AppendLine(')WFWEB";')
    [void]$sb.AppendLine("")
}

[void]$sb.AppendLine("#endif // WIND_FOLLOW_WEB_ASSETS_H")

$utf8 = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllText($OutFile, $sb.ToString(), $utf8)
Write-Output ("Gerado: {0} ({1} bytes)" -f $OutFile, (Get-Item -LiteralPath $OutFile).Length)
