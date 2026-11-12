# Funcao para ler arquivo como texto sem BOM
function Get-ContentNoBom {
    param([string]$Path)
    $bytes = [System.IO.File]::ReadAllBytes($Path)
    # Remove BOM se presente (EF BB BF)
    if ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF) {
        $bytes = $bytes[3..$bytes.Length]
    }
    $enc = New-Object System.Text.UTF8Encoding $false
    return $enc.GetString($bytes)
}

# Funcao para escrever arquivo UTF-8 sem BOM
function Set-ContentNoBom {
    param([string]$Path, [string]$Value)
    $enc = New-Object System.Text.UTF8Encoding $false
    [System.IO.File]::WriteAllText($Path, $Value, $enc)
}

$WebDir = "web"
$OutFile = "WebAssets.h"

if (!(Test-Path -LiteralPath $WebDir)) {
    Write-Error "Diretorio web/ nao encontrado em $PWD"
    exit 1
}

$files = @(
    @{ name = "index_html"; path = Join-Path $WebDir "index.html" }
    @{ name = "style_css";  path = Join-Path (Join-Path $WebDir "css") "style.css" }
    @{ name = "app_js";     path = Join-Path (Join-Path $WebDir "js") "app.js" }
)

$header = @"
#ifndef WEB_ASSETS_H
#define WEB_ASSETS_H

// Conteudos gerados automaticamente a partir da pasta web/
// Nao edite manualmente - rode generate_web_assets.ps1 apos alterar os arquivos web/

"@

$footer = @"

#endif // WEB_ASSETS_H
"@

$content = $header

foreach ($f in $files) {
    if (!(Test-Path -LiteralPath $f.path)) {
        Write-Error "Arquivo nao encontrado: $($f.path)"
        exit 1
    }
    $raw = Get-ContentNoBom $f.path
    # Escapa para raw string literal C++ (R"rawliteral(...)rawliteral")
    # Substitui ))rawliteral( se houver (improvisavel aqui, mas protegido)
    $content += "static const char* $($f.name) = R`"rawliteral`(`"`n"
    $content += $raw
    $content += "`n)rawliteral`";`n"
}

$content += $footer

Set-ContentNoBom -Path $OutFile -Value $content
Write-Host "Gerado $OutFile com sucesso ($( $files.Length ) arquivos embutidos)"
