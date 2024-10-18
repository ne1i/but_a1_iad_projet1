app.exe<in-out/avec-erreurs/in-sp1.txt>out.txt
fc /w /n out.txt in-out/avec-erreurs/out-sp1.txt
app.exe<in-out/avec-erreurs/in-sp2.txt>out.txt
fc /w /n out.txt in-out/avec-erreurs/out-sp2.txt
app.exe<in-out/avec-erreurs/in-sp3.txt>out.txt
fc /w /n out.txt in-out/avec-erreurs/out-sp3.txt
app.exe<in-out/avec-erreurs/in-sp4.txt>out.txt
fc /w /n out.txt in-out/avec-erreurs/out-sp4.txt
pause
app.exe<in-out/sans-erreur/in-sp1.txt>out.txt
fc /w /n out.txt in-out/sans-erreur/out-sp1.txt
app.exe<in-out/sans-erreur/in-sp2.txt>out.txt
fc /w /n out.txt in-out/sans-erreur/out-sp2.txt
app.exe<in-out/sans-erreur/in-sp3.txt>out.txt
fc /w /n out.txt in-out/sans-erreur/out-sp3.txt
app.exe<in-out/sans-erreur/in-sp4.txt>out.txt
fc /w /n out.txt in-out/sans-erreur/out-sp4.txt
pause