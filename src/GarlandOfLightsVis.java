import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.util.*;
import java.security.*;
import javax.swing.*;
import javax.imageio.*;

// --------------------------------------------------------
public class GarlandOfLightsVis {
    static int maxS = 100, minS = 10;
    static int maxC = 4, minC = 2;
    static int dr[] = { -1, 1, 0, 0 }, dc[] = { 0, 0, -1, 1 };
    static int typeDirs[][] = { { 1, 3 }, { 1, 2 }, { 0, 2 }, { 0, 3 }, { 2, 3 }, { 0, 1 } };
    // ---------------------------------------------------
    int H, W; // space size
    int C; // colors used for the lights
    int N; // number of lights
    int[] lightTypes; // input lights types
    char[] lightColors; // input lights colors
    String[] lightsArg; // input lights: type 0..5 + color a..d
    int[] lightsRet; // a permutation of numbers 0..H*W-1 - the light used in appropriate position
    boolean[] longestLoop; // whether tile in slot i is part of the longest loop (for visualization)
    // ---------------------------------------------------

    String generate(long seed) {
        try {
            SecureRandom rnd = SecureRandom.getInstance("SHA1PRNG");
            rnd.setSeed(seed);

            H = rnd.nextInt(maxS - minS + 1) + minS;
            W = rnd.nextInt(maxS - minS + 1) + minS;
            C = rnd.nextInt(maxC - minC + 1) + minC;
            if (seed == 1) {
                H = W = 5;
                C = maxC;
            } else if (seed == 2) {
                H = 5;
                W = 10;
                C = minC;
            } else if (seed == 3) {
                H = W = maxS;
                C = maxC;
            } else if (seed == 4) {
                H = W = maxS;
                C = minC;
            }

            if (vis) {
                SZX = W * SZ;
                SZY = H * SZ;
            }

            // generate the array of input lights (types)
            N = W * H;
            lightTypes = new int[N];
            for (int i = 0; i < N; ++i) {
                lightTypes[i] = rnd.nextInt(6);
            }
            // generate the colors of input lights
            // verify that each color is used at least once
            lightColors = new char[N];
            while (true) {
                int[] nColors = new int[C];
                for (int i = 0; i < N; ++i) {
                    int col = rnd.nextInt(C);
                    lightColors[i] = (char) (col + 'a');
                    nColors[col]++;
                }
                boolean ok = true;
                for (int i = 0; i < C; ++i)
                    if (nColors[i] == 0)
                        ok = false;
                if (ok)
                    break;
            }

            // convert to string argument
            lightsArg = new String[N];
            for (int i = 0; i < N; ++i) {
                lightsArg[i] = lightTypes[i] + "" + lightColors[i];
            }

            StringBuffer sb = new StringBuffer();
            sb.append("H = ").append(H).append('\n');
            sb.append("W = ").append(W).append('\n');
            sb.append("C = ").append(C).append('\n');
            if (seed < 3)
                for (int i = 0; i < N; ++i)
                    sb.append(lightsArg[i]).append((i + 1) % W > 0 ? ' ' : '\n');
            return sb.toString();
        } catch (Exception e) {
            addFatalError("An exception occurred while generating test case.");
            e.printStackTrace();
            return "";
        }
    }

    // ---------------------------------------------------
    public double runTest(long seed) {
        try {
            String test = generate(seed);
            if (debug)
                System.out.println(test);

            // call user's solution and get return
            lightsRet = new int[N];
            if (proc != null) {
                try {
                    lightsRet = create(H, W, lightsArg);
                } catch (Exception e) {
                    addFatalError("Failed to get result from create.");
                    return 0;
                }

                if (lightsRet.length != N) {
                    addFatalError(
                            "Your return must contain " + N + " elements, but it contained " + lightsRet.length + ".");
                    return 0;
                }

                // check that this is a valid permutation
                boolean[] usedP = new boolean[N];
                for (int i = 0; i < N; ++i) {
                    if (lightsRet[i] < 0 || lightsRet[i] >= N) {
                        addFatalError("All elements of your return must be between 0 and " + (N - 1)
                                + ", and your return contained " + lightsRet[i] + ".");
                        return 0;
                    }
                    if (usedP[lightsRet[i]]) {
                        addFatalError("All elements of your return must be unique, and your return contained "
                                + lightsRet[i] + " twice.");
                        return 0;
                    }
                    usedP[lightsRet[i]] = true;
                }
            } else {
                // use the starting layout
                for (int i = 0; i < N; ++i)
                    lightsRet[i] = i;
            }

            if (debug) {
                StringBuffer sb = new StringBuffer();
                for (int i = 0; i < N; ++i)
                    sb.append(lightsArg[lightsRet[i]]).append((i + 1) % W > 0 ? ' ' : '\n');
                System.out.println("Return grid:");
                System.out.println(sb.toString());
            }

            // find the longest valid loop of garland tiles
            if (vis) {
                longestLoop = new boolean[N];
            }
            int maxLen = 0;
            boolean[] used = new boolean[N];
            boolean[] curLoop = new boolean[N];
            // iterate through positions in the grid in row-wise order
            for (int start = 0; start < N; ++start) {
                if (used[start])
                    continue;
                boolean goodLoop = true, goodColors = true;
                int len = 0;
                int startR = start / W;
                int startC = start % W;
                Arrays.fill(curLoop, false);
                curLoop[start] = true;
                // track the garland starting at this tile
                StringBuffer sb = new StringBuffer();
                int startLight = lightsRet[start];
                int startType = lightTypes[startLight];
                // go in one direction at a time (clockwise first) unless it's a loop
                for (int d = 1; d >= 0; d--) {
                    int r = startR, c = startC;
                    if (d == 1)
                        sb.append("(" + r + "," + c + ")");
                    int curColor = lightColors[startLight];
                    int curDir = typeDirs[startType][d];
                    while (true) {
                        // move in the direction diven by curDir
                        int nextR = r + dr[curDir];
                        int nextC = c + dc[curDir];
                        int nextCell = nextR * W + nextC;
                        if (nextR < 0 || nextR == H || nextC < 0 || nextC == W || used[nextCell]) {
                            // the direction leads outside the border or to a previously used cell - not a loop
                            goodLoop = false;
                            break;
                        }
                        int nextLight = lightsRet[nextCell];
                        int nextType = lightTypes[nextLight];

                        // check whether next tile is connected to previous one
                        int nextDir = -1;
                        if (typeDirs[nextType][0] == (curDir ^ 1))
                            nextDir = typeDirs[nextType][1];
                        else if (typeDirs[nextType][1] == (curDir ^ 1))
                            nextDir = typeDirs[nextType][0];
                        else {
                            // next tile is not connected to the previous one - not a loop
                            goodLoop = false;
                            break;
                        }

                        // next tile is connected to the previous one - can continue
                        // mark next tile as part of the loop
                        len++;
                        used[nextCell] = true;
                        curLoop[nextCell] = true;
                        sb.append(", (" + nextR + "," + nextC + ")");

                        // check the colors
                        if (curColor == lightColors[nextLight]) {
                            // two of the same color in row - mark as bad but continue along the loop
                            goodColors = false;
                        }
                        if (nextR == startR && nextC == startC)
                            break;

                        // next is now current
                        r = nextR;
                        c = nextC;
                        curColor = lightColors[nextLight];
                        curDir = nextDir;
                    }

                    if (goodLoop)
                        break;
                }

                if (debug && (chains || goodLoop)) {
                    System.out.print(goodLoop && goodColors ? "Good loop" : (goodLoop ? "Bad loop" : "Chain"));
                    System.out.println(" of length " + (goodLoop ? len : len + 1) + ": " + sb.toString());
                }
                if (goodLoop && goodColors) {
                    // we returned to the starting tile with all connected tiles and alternating colors
                    if (len > maxLen) {
                        maxLen = len;
                        if (vis) {
                            longestLoop = Arrays.copyOf(curLoop, N);
                        }
                    }
                }
            }

            if (vis) {
                // draw the image
                jf.setSize(SZX + 10, SZY + 30);
                jf.setVisible(true);
                draw();
            }

            return maxLen * 1.0 / N;
        } catch (Exception e) {
            addFatalError("An exception occurred while trying to process your program's results.");
            e.printStackTrace();
            return -1;
        }
    }

    // ------------- visualization part ----------------------
    static String exec, fileName;
    static boolean vis, debug, save, chains;
    static Process proc;
    JFrame jf;
    Vis v;
    InputStream is;
    OutputStream os;
    BufferedReader br;
    // problem-specific drawing params
    int SZX, SZY;
    static int SZ;

    // ---------------------------------------------------
    int[] create(int H, int W, String[] lights) throws IOException {
        // pass the params to the solution and get the return
        int i;
        StringBuffer sb = new StringBuffer();
        sb.append(H).append('\n');
        sb.append(W).append('\n');
        for (i = 0; i < H * W; ++i)
            sb.append(lights[i]).append('\n');
        os.write(sb.toString().getBytes());
        os.flush();

        // get the return - an array of integers
        int nret = Integer.parseInt(br.readLine());
        int[] ret = new int[nret];
        for (i = 0; i < nret; ++i)
            ret[i] = Integer.parseInt(br.readLine());
        return ret;
    }

    // ---------------------------------------------------
    void draw() {
        if (!vis)
            return;
        v.repaint();
    }

    // ---------------------------------------------------
    int[] colors = { 0xE43BA6, 0x00BCF2, 0xFCE100, 0x7CD300 };

    // ---------------------------------------------------
    public class Vis extends JPanel implements WindowListener {
        public void paint(Graphics g) {
            try {
                // do painting here
                BufferedImage bi = new BufferedImage(SZX + 5, SZY + 5, BufferedImage.TYPE_INT_RGB);
                Graphics2D g2 = (Graphics2D) bi.getGraphics();
                // background
                g2.setColor(new Color(0xE3E3E3));
                g2.fillRect(0, 0, SZX + 10, SZY + 10);
                FontMetrics fm = g2.getFontMetrics();

                // white background for cells which are part of the longest loop
                g2.setColor(Color.WHITE);
                for (int r = 0; r < H; ++r)
                    for (int c = 0; c < W; ++c) {
                        if (longestLoop[r * W + c])
                            g2.fillRect(c * SZ, r * SZ, SZ, SZ);
                    }

                // lines between cells
                g2.setColor(new Color(0xAAAAAA));
                for (int i = 0; i <= H; i++)
                    g2.drawLine(0, i * SZ, W * SZ, i * SZ);
                for (int i = 0; i <= W; i++)
                    g2.drawLine(i * SZ, 0, i * SZ, H * SZ);

                // wires on tiles
                g2.setColor(Color.BLACK);
                int d = 0;
                if (SZ > 12) {
                    g2.setStroke(new BasicStroke(3.0f));
                    d = 1;
                }
                for (int r = 0; r < H; ++r)
                    for (int c = 0; c < W; ++c) {
                        int type = lightTypes[lightsRet[r * W + c]];
                        for (int k = 0; k < 2; ++k) {
                            g2.drawLine(c * SZ + SZ / 2, r * SZ + SZ / 2,
                                    c * SZ + SZ / 2 + dc[typeDirs[type][k]] * (SZ / 2 - d),
                                    r * SZ + SZ / 2 + dr[typeDirs[type][k]] * (SZ / 2 - d));
                        }
                    }

                for (int r = 0; r < H; ++r)
                    for (int c = 0; c < W; ++c) {
                        int color = lightColors[lightsRet[r * W + c]] - 'a';
                        g2.setColor(new Color(colors[color]));
                        g2.fillOval(c * SZ + SZ / 2 - SZ / 4, r * SZ + SZ / 2 - SZ / 4, SZ / 2, SZ / 2);
                    }

                g.drawImage(bi, 0, 0, SZX + 5, SZY + 5, null);
                if (save) {
                    ImageIO.write(bi, "png", new File(fileName + ".png"));
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        public Vis() {
            jf.addWindowListener(this);
        }

        // WindowListener
        public void windowClosing(WindowEvent e) {
            if (proc != null)
                try {
                    proc.destroy();
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            System.exit(0);
        }

        public void windowActivated(WindowEvent e) {
        }

        public void windowDeactivated(WindowEvent e) {
        }

        public void windowOpened(WindowEvent e) {
        }

        public void windowClosed(WindowEvent e) {
        }

        public void windowIconified(WindowEvent e) {
        }

        public void windowDeiconified(WindowEvent e) {
        }
    }

    // ---------------------------------------------------
    public GarlandOfLightsVis(long seed) {
        //interface for runTest
        if (vis) {
            jf = new JFrame();
            v = new Vis();
            jf.getContentPane().add(v);
        }
        if (exec != null) {
            try {
                Runtime rt = Runtime.getRuntime();
                proc = rt.exec(exec);
                os = proc.getOutputStream();
                is = proc.getInputStream();
                br = new BufferedReader(new InputStreamReader(is));
                new ErrorReader(proc.getErrorStream()).start();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        System.out.println("Score = " + runTest(seed));
        if (proc != null)
            try {
                proc.destroy();
            } catch (Exception e) {
                e.printStackTrace();
            }
    }

    // ---------------------------------------------------
    public static void main(String[] args) {
        long seed = 1;
        SZ = 12;
        for (int i = 0; i < args.length; i++) {
            if (args[i].equals("-seed"))
                seed = Long.parseLong(args[++i]);
            if (args[i].equals("-exec"))
                exec = args[++i];
            if (args[i].equals("-vis"))
                vis = true;
            if (args[i].equals("-debug"))
                debug = true;
            if (args[i].equals("-size"))
                SZ = Integer.parseInt(args[++i]);
            if (args[i].equals("-save"))
                save = true;
            if (args[i].equals("-chains"))
                chains = true;
        }
        if ((seed == 1 || seed == 2) && SZ < 30)
            SZ = 30;
        if (chains)
            debug = true;
        if (save) {
            fileName = String.valueOf(seed);
            vis = true;
        }
        new GarlandOfLightsVis(seed);
    }

    // ---------------------------------------------------
    void addFatalError(String message) {
        System.out.println(message);
    }
}

class ErrorReader extends Thread {
    InputStream error;

    public ErrorReader(InputStream is) {
        error = is;
    }

    public void run() {
        try {
            byte[] ch = new byte[50000];
            int read;
            while ((read = error.read(ch)) > 0) {
                String s = new String(ch, 0, read);
                System.out.print(s);
                System.out.flush();
            }
        } catch (Exception e) {
        }
    }
}
