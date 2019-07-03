%most q3d files where ones saved into .m scripts this script transforms them all to .mat files 
files = dir('../q3d_res/*.m');
cd ../q3d_res/
for file =files'
    run(file.name)
    save(strcat(file.name,'at'),'capMatrix','spicecapMatrix')
    
end
%run(file.name)