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

namespace LayoutOfADisconnectedGraphWithSugiyama {

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


    public partial class Form1 : Form {



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
                if (System.Text.RegularExpressions.Regex.IsMatch(line, ".* Function.*"))
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




        public Form1() {
#if DEBUG
            DisplayGeometryGraph.SetShowFunctions();
#endif

            InitializeComponent();
            SetGraph();
        }

        private void SetGraph() {

            string path = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), @"crazyPointer.pointer.log");

            List<IR_Cluster> IR_List = new List<IR_Cluster>();
            readFile(path, IR_List);



            var graph = new Graph();
            graph.AddEdge("a", "b");
            graph.AddEdge("e", "b");
            graph.AddEdge("d", "b");
            graph.AddEdge("b", "c");

            graph.AddEdge("a22", "b22");
            graph.AddEdge("e22", "b22");
            graph.AddEdge("d22", "b22");
            graph.AddEdge("b22", "c22");

            graph.AddEdge("a0", "b0");
            graph.AddEdge("b0", "c0");

            graph.AddEdge("a33", "b33");
            graph.AddEdge("e33", "b33");
            graph.AddEdge("d33", "b33");
            graph.AddEdge("b33", "c33");

            graph.AddEdge("a11", "b11");
            graph.AddEdge("b11", "c11").LabelText = "Test labels!";

            graph.CreateGeometryGraph();
            foreach (Node node in graph.Nodes)
                node.GeometryNode.BoundaryCurve = CreateLabelAndBoundary(node);

            foreach (var edge in graph.Edges) {
                if (edge.Label != null) {
                    var geomEdge = edge.GeometryEdge;
                    double width;
                    double height;
                    StringMeasure.MeasureWithFont(edge.LabelText,
                                                  new Font(edge.Label.FontName, (float)edge.Label.FontSize), out width, out height);
                    edge.Label.GeometryLabel=geomEdge.Label = new Label(width, height, geomEdge);                    
                }

            }

            var geomGraph=graph.GeometryGraph;

            var geomGraphComponents = GraphConnectedComponents.CreateComponents(geomGraph.Nodes, geomGraph.Edges);
            var settings = new SugiyamaLayoutSettings();
            foreach (var subgraph in geomGraphComponents) {
               
                var layout=new LayeredLayout(subgraph, settings);
                subgraph.Margins = settings.NodeSeparation/2;
                layout.Run();

            }

           Microsoft.Msagl.Layout.MDS.MdsGraphLayout.PackGraphs(geomGraphComponents, settings);

            geomGraph.UpdateBoundingBox();


            gViewer1.NeedToCalculateLayout = false;
            gViewer1.Graph = graph;

        }

        static ICurve CreateLabelAndBoundary(Node node) {
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
    }
}
