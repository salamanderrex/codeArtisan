using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using Microsoft.Msagl.Core.Geometry.Curves;
using Microsoft.Msagl.Core.Layout;
using Microsoft.Msagl.DebugHelpers;
using Microsoft.Msagl.Drawing;
using Microsoft.Msagl.GraphViewerGdi;
using Microsoft.Msagl.Layout.Initial;
using Microsoft.Msagl.Layout.Layered;
using Label = Microsoft.Msagl.Core.Layout.Label;
using Node = Microsoft.Msagl.Drawing.Node;
using Point = Microsoft.Msagl.Core.Geometry.Point;
using System.ComponentModel;
using System.IO;
using System.Reflection;
using System.Text.RegularExpressions;

namespace PhyloTreeSampleOverGDI
{
    public partial class Form1 : Form
    {
        GViewer viewer = new GViewer();
        List<IR_Cluster> IR_List = new List<IR_Cluster>();


        int counter = 0;


        public Form1()
        {

            
            string path = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), @"crazyPointer.pointer.log");
           // codeBox.Text = System.IO.File.ReadAllText(@"crazyPointer.pointer.log");
            
            readFile(path, IR_List);


            InitializeComponent();
            SuspendLayout();
            Controls.Add(viewer);
            viewer.Dock = DockStyle.Fill;
            viewer.LayoutAlgorithmSettingsButtonVisible = false;
            ResumeLayout();
            codeBox.Lines = File.ReadAllLines(@"crazyPointer.cpp");




        }


        Microsoft.Msagl.Drawing.Color getAcolor(int counter)
        {
            if (counter % 3 == 0)
            {
                return Microsoft.Msagl.Drawing.Color.Green;
            }
            else if (counter % 3 == 1)
            {
                return Microsoft.Msagl.Drawing.Color.PowderBlue;
            }
            else if (counter % 3 == 2)
            {
                return Microsoft.Msagl.Drawing.Color.Yellow;
            }

            return Microsoft.Msagl.Drawing.Color.Purple;
        }


        void button1_Click(object sender, EventArgs e)
        {
            button_ClickBase(sender, e, 0);

        }


        void button2_click(object sender, EventArgs e)
        {
            button_ClickBase(sender, e, 1);

        }


        void button_ClickBase(object sender, EventArgs e, int counterDirection)
        {

            if (counter >= IR_List.Count())
            {
                counter = IR_List.Count() - 1;
            }
            if (counter < 0)
            {
                counter = 0;
            }



            var graph = new Graph();

            Console.WriteLine("in the buttion1!~!!!!!");


            IR_Cluster irCluster = IR_List[counter];
            if (counterDirection == 0)
            {
                counter++;
                counter++;
            }
            else
            {
                counter--;
            }
            Console.WriteLine("conuter is " + counter);
            //graph.AddEdge("instruction", "instruction").LabelText = irCluster.cluster[0].Instname;
            //Node Insnode = new Node(irCluster.cluster[0].Instname);
            this.textBox1.Text = irCluster.cluster[0].Instname;
            // graph.AddNode(Insnode);

            string previousP = "";
            int color = 0;

            List<KeyValuePair<String, List<string>>> coloring = new List<KeyValuePair<string, List<string>>>();
            List<string> currColorList = new List<string>();
            foreach (IR ir in irCluster.cluster)
            {

                Console.WriteLine(ir.PointerStatus);
                String startP = ir.returnStartP();
                String endP = ir.returnEndP();
                //graph.AddEdge(startP, endP);
                if (!startP.Equals(previousP))
                {
                    if (previousP != "" && currColorList.Count() > 0)
                    {
                        coloring.Add(new KeyValuePair<String, List<string>>(previousP, currColorList));
                    }
                    currColorList = new List<string>();
                    color++;
                }
                previousP = startP;
                if (startP.Equals(endP) && (startP.Equals("BOTTOM") || startP.Equals("TOP")))
                {
                    //BTOOOM or TOP
                    Node node = new Node(startP);
                    graph.AddNode(node);
                }
                else
                {
                    currColorList.Add(endP);
                    graph.AddEdge(startP, endP);
                    Microsoft.Msagl.Drawing.Node c = graph.FindNode(startP);
                    c.Attr.Shape = Microsoft.Msagl.Drawing.Shape.Circle;
                    graph.FindNode(startP).Attr.FillColor = getAcolor(color);
                    graph.FindNode(endP).Attr.FillColor = getAcolor(color);


                    //check end pointer shared by others

                    foreach (KeyValuePair<String, List<string>> kvp in coloring)
                    {
                        List<string> tmpList = kvp.Value;
                        foreach (string s in tmpList)
                        {
                            if (s.Equals(endP))
                            {
                                graph.FindNode(endP).Attr.FillColor = Microsoft.Msagl.Drawing.Color.Red;
                            }
                        }
                    }

                }

            }

            viewer.Graph = graph;



        }


        static ICurve CreateLabelAndBoundary(Node node)
        {
            node.Attr.Shape = Shape.DrawFromGeometry;
            node.Attr.LabelMargin *= 2;
            double width;
            double height;
            StringMeasure.MeasureWithFont(node.Label.Text,
                                          new Font(node.Label.FontName, (float)node.Label.FontSize), out width, out height);
            node.Label.Width = width;
            node.Label.Height = height;
            int r = node.Attr.LabelMargin;
            return CurveFactory.CreateRectangleWithRoundedCorners(width + r * 2, height + r * 2, r, r, new Point());
        }



        static void readFile(String filename, List<IR_Cluster> IR_List)
        {


            string functionName = " ";
            string direction = " ";
            string Instname = " ";
            string pointerStatus = " ";
            IR_Cluster IRcluster = new IR_Cluster();
            string[] lines = System.IO.File.ReadAllLines(filename);

            // Display the file contents by using a foreach loop.
            System.Console.WriteLine("Contents of WriteLines2.txt = ");

            for (int i = 0; i < lines.Length; i++)
            {
                System.Console.WriteLine(lines[i]);
            }

            for (int i = 0; i < lines.Length; i++)
            {
                // Use a tab to indent each line of the file.
                string line = lines[i];
                // Console.WriteLine("\t" + line);
                line = line.Trim();
                if (line == null || line == " " || line == "\n" || line == "=======" || line == "\r\n")
                {
                    continue;
                }

                if (!System.Text.RegularExpressions.Regex.IsMatch(line, "\\S"))
                {

                    continue;
                }
                if (System.Text.RegularExpressions.Regex.IsMatch(line, ".*Function.*"))
                {
                    functionName = line;
                    System.Console.WriteLine("function is " + functionName);
                    continue;
                }


                if (System.Text.RegularExpressions.Regex.IsMatch(line, ".*tation.*"))
                {
                    Instname = line;
                    System.Console.WriteLine("Instruction name  is " + Instname);
                    continue;
                }

                if (System.Text.RegularExpressions.Regex.IsMatch(line, ".*ing.*"))
                {
                    direction = line;
                    System.Console.WriteLine("direction is " + direction);
                    continue;
                }

                pointerStatus = line;
                System.Console.WriteLine("pointerStatus is " + pointerStatus);

                IR ir = new IR(functionName, direction, Instname, pointerStatus);

                if (IRcluster.memberOrNot(ir))
                {
                    IRcluster.cluster.Add(ir);
                }
                else
                {
                    IR_List.Add(IRcluster);
                    IRcluster = new IR_Cluster();
                    IRcluster.cluster.Add(ir);
                }




            }

            // Keep the console window open in debug mode.
            /*
            Console.WriteLine("SUMMERY ==============");
            foreach (PropertyDescriptor descriptor in TypeDescriptor.GetProperties(IR_List))
            {
                string name = descriptor.Name;
                object value = descriptor.GetValue(IR_List);
                Console.WriteLine("{0}={1}", name, value);
            }

    */

        }







        public class IR
        {
            /*
            def __init__(self, FunctionName, Instname, direction, pointerStatus):
            self.functionName = FunctionName
            self.direction = direction
            self.Instname = Instname
            self.pointerStatus = pointerStatus
            */

            public String functionName { get; set; }
            public String direction { get; set; }  //incoming or outcoming
            public String Instname { get; set; }
            public String PointerStatus { get; set; }

            public IR(String functionName, String direction, String Instname, String PointerStatus)
            {
                this.functionName = functionName;
                this.direction = direction;
                this.Instname = Instname;
                this.PointerStatus = PointerStatus;
            }

            public string returnStartP()
            {


                string returnStr = PointerStatus; // for Bottom
                string pattern = @"(?<word>\w+)->\w+";

                Regex rgx = new Regex(pattern, RegexOptions.IgnoreCase);
                MatchCollection matches = rgx.Matches(PointerStatus);
                if (matches.Count > 0)
                {

                    //Console.WriteLine("{0} ({1} matches):", PointerStatus, matches.Count);
                    foreach (Match match in matches)
                    {
                        GroupCollection groups = match.Groups;
                        // Console.WriteLine(" \naddsdas`  " + match.Value + "  " + groups["word"].Value);
                        returnStr = groups["word"].Value;
                    }
                }

                Console.WriteLine(returnStr);
                return returnStr;
            }

            public string returnEndP()
            {


                string returnStr = PointerStatus; // for Bottom
                string pattern = @"\w+->(?<word>\w+)";


                Regex rgx = new Regex(pattern, RegexOptions.IgnoreCase);
                MatchCollection matches = rgx.Matches(PointerStatus);
                if (matches.Count > 0)
                {

                    //Console.WriteLine("{0} ({1} matches):", PointerStatus, matches.Count);
                    foreach (Match match in matches)
                    {
                        GroupCollection groups = match.Groups;
                        // Console.WriteLine(" \naddsdas`  " + match.Value + "  " + groups["word"].Value);
                        returnStr = groups["word"].Value;
                    }
                }

                Console.WriteLine(returnStr);
                return returnStr;
            }
        }

        public class IR_Cluster
        {
            public List<IR> cluster = new List<IR>();
            public Boolean memberOrNot(IR ir)
            {
                if (cluster.Count == 0)
                {
                    return true;
                }

                if (ir.functionName.Equals(cluster[0].functionName) &&
                     ir.direction.Equals(cluster[0].direction) &&
                     ir.Instname.Equals(cluster[0].Instname))
                {
                    return true;
                }
                return false;
            }


        }
    }
}