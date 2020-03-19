FROM mcr.microsoft.com/windows:1903

RUN choco install sccache curl 7zip.portable
RUN curl https://endless-sky.github.io/win64-dev.zip -o dev64.zip && 7z x dev64.zip -o C:\ && del dev64.zip