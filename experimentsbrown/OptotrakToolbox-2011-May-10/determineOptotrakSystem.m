%Name:             determineOptotrakSystem.m

%Description:

% - Initializes the Optotrak system and writes the files system.nif
%   and system.cam
%
% NOTE: You could also do this using the NDI ToolBench, but it might
% be more convenient to do it from within MATLAB. You MUST rerun this
% whenever you changed the cabling of the Optotrak (see the Optotrak
% users guide and the description of TransputerDetermineSystemCfg).

%Just to be on the save side, we first reset all Matlab functions:
clear functions

fprintf('\n')
fprintf('Determins system configuration and writes the files system.nif and system.cam \n')
fprintf('to the standard ndigital directory. (e.g.: c:/ndigital/realtime)\n')
fprintf('\n')
fprintf('NOTE: You could also do this using the NDI ToolBench, but it might\n')
fprintf('be more convenient to do it from within MATLAB. You MUST run this\n')
fprintf('whenever you changed the cabling of the Optotrak (see the Optotrak\n')
fprintf('users guide and the description of TransputerDetermineSystemCfg).\n')
fprintf('\n')

optotrak('TransputerDetermineSystemCfg');
fprintf('... done\n');

