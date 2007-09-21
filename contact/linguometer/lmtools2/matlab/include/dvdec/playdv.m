%PLAYDV  Displays DV movie
%   PLAYDV('filename') plays a DV movie file in a figure.  While
%   playing, press SPACE to pause/unpause, 'q' to quit, 'n' to
%   step forward one frame.  The timecode encoded in the video
%   will be displayed in the upper left.
function playdv(str);

dv = dvopen(str);
h = figure('KeyPressFcn', @figkeypress);
guidata(h, 0);
dopause = 1;
set(h, 'DoubleBuffer', 'on');
while 1,
    [i, t] = dvread(dv);
    if size(i,1) == 0,
        break;
    end
    ih = image(i);
    s = sprintf('%02d:%02d:%02d.%02d', t(1), t(2), t(3), t(4));
    text(20,20,s,'FontName', 'Courier', 'FontSize', 12, 'FontWeight', 'bold', 'VerticalAlignment', 'top', 'Color', [1 1 1])
    title('Playing DV file -- press Q to quit');
    axis equal;
    drawnow;
    if (guidata(h) == 'q')
        fprintf('quitting...\n');
        break;
    end
    if (guidata(h) == ' ')
        dopause = 1;
    end
    guidata(h, 0);
    while dopause,
        pause(1/30);
        if (guidata(h) == 'n')
            break;
        end
        if (guidata(h) == ' ')
            dopause = 0;
        end
    end
    guidata(h, 0);
end
dvclose(dv);

function figkeypress(src,evnt)
guidata(src,get(src, 'CurrentCharacter'));

