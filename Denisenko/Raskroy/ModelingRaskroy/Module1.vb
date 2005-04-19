Imports System.Console
Imports System.Threading.Thread
Imports System.Threading

Module Module1

    'Class Part
    'Public Length As Long
    'Public Width As Long
    'Public Ammount As Long
    'Public Rotate As Boolean
    'End Class

    'Class Sheet
    '    Public Length As Long
    '    Public Width As Long
    'End Class

    Dim CutWidth As Double

    Class Routine
        Public Server As String
        Public Parts As New COMRASKROYLib.SheetsClass()
        Public Sheets As New COMRASKROYLib.SheetsClass()
        Public Results() As COMRASKROYLib.Result
        Public Rashod As Long
        Public Color As String
        Public dsp As Boolean

        Sub Run()
            Dim x As COMRASKROYLib.IRaskroy
            x = CreateObject("COMRASKROYLib.Raskroy", Server)
            Dim res As COMRASKROYLib.Result
            If x.First(Parts, Sheets, res) Then
                Do
                    Results(Results.Length) = res
                    Rashod = Rashod + res.Amount
                Loop While x.Next(res)
            End If
        End Sub
    End Class

    Function SelectServer() As String
        Static servers(6) As String
        Static cntr As Byte
        servers(0) = "Computer"
        servers(1) = "SERVER"
        servers(2) = "Tanya"
        servers(3) = "Sveta"
        servers(4) = "Popova"
        servers(5) = "Black"
        SelectServer = servers(cntr)
        'cntr = cntr + 1
        'If cntr > 5 Then cntr = 0
    End Function

    Sub Main()
        Dim x1 As COMRASKROYLib.IRaskroy
        x1 = CreateObject("COMRaskroy.Raskroy")

        Dim col As New Collection()
        ' �������� ������ �� ���� � ������� ������ �� ���������� � ������
        Dim acc As Object
        acc = GetObject("\\server\pubs\�������\����������.mdb")
        'acc.OpenCurrentDatabase("\\server\pubs\db1.mdb")

        ' ������ ������ ���������� � ������ rs(0) - �������� (���/������)
        ' rs(1) - ����
        Dim rs As DAO.Recordset
        Dim rs1 As DAO.Recordset
        rs = acc.CurrentDb.OpenRecordset("SELECT ���, ���� FROM ������ INNER JOIN ������ ON ������.���=������.��������� WHERE ���=True GROUP BY ���, ����")
        Dim ������ = 4

        ' ��� ������ ���� (��������,����) ��������� ������
        While Not rs.EOF
            Dim x As New Routine()
            col.Add(x)
            x.dsp = rs(0).Value
            'If x.dsp Then
            '    x.Sheets.Add2(1810, 1210, 1000, False)
            'End If
            x.Sheets.Add2(1810, 1210, 1000, False)
            'x.Sheets.Add2(2440, 1210, 1000, False)
            x.Color = rs(1).Value
            x.Server = SelectServer()

            ' ����������� ������ ��� ���������,�����
            ' rs1(0) - �����, rs1(1) - ������, rs1(2) - ���-��,
            ' rs1(3) - �������
            rs1 = acc.CurrentDb.OpenRecordset("SELECT �����, ������, ����������, ������� FROM (������ INNER JOIN ������ ON ������.���������=������.���) LEFT JOIN ����� ON ������.����=�����.������������ WHERE ���=" & x.dsp & " AND ����=""" & x.Color & """")
            Dim count As Integer
            count = 0
            While Not rs1.EOF
                x.Parts.Add2(rs1(0).Value, rs1(1).Value, rs1(2).Value * ������, rs1(3).Value)
                rs1.MoveNext()
                'count = count + 1
                'If count = 10 Then Exit While
            End While
            Console.WriteLine("Loaded parts for color " & x.Color & ", dsp " & x.dsp & ", amount " & rs1.RecordCount)
            rs.MoveNext()
        End While

        ' ������ ����������� ��������
        Dim element As Routine
        Dim r As New COMRASKROYLib.Raskroy()
        r.RecursionDepth = 1000
        rs = acc.CurrentDb.OpenRecordset("SELECT ���, ����� FROM �������")
        rs.AddNew()
        Dim ���������� As Integer
        ���������� = acc.CurrentDb.OpenRecordset("SELECT Nz(LAST(���))+1 FROM (SELECT * FROM ������� ORDER BY ���)")(0).Value
        rs.Fields(0).Value = ����������
        rs.Fields(1).Value = Now()
        rs.Update()
        rs = acc.CurrentDb.OpenRecordset("SELECT ���, ����������, ���� FROM ���������������")
        rs1 = acc.CurrentDb.OpenRecordset("SELECT ������������������, Value, ����������, ����� FROM ���������������������")
        For Each element In col
            rs.AddNew()
            Dim ����������������� As Integer
            ����������������� = acc.CurrentDb.OpenRecordset("SELECT Nz(LAST(���))+1 FROM (SELECT * FROM ��������������� ORDER BY ���)")(0).Value
            rs.Fields(0).Value = �����������������
            rs.Fields(1).Value = ����������
            rs.Fields(2).Value = element.Color
            rs.Update()
            Dim SumScrap = 0
            Dim res As COMRASKROYLib.Result
            If r.First(element.Parts, element.Sheets, res) Then
                Do
                    r.RecursionDepth = r.RecursionDepth + 2
                    rs1.AddNew()
                    rs1.Fields(0).Value = �����������������
                    rs1.Fields(1).Value = res.String
                    rs1.Fields(2).Value = res.Amount
                    Dim Scrap = res.UsefulScrap + res.Opilki + res.UnUsefulScrap
                    rs1.Fields(3).Value = Scrap
                    SumScrap = SumScrap + Scrap * res.Amount
                    rs1.Update()
                    element.Rashod = element.Rashod + res.Amount
                    Console.Write(".")
                Loop While r.Next(res)
            End If
            'Dim y As New System.Threading.Thread(AddressOf element.Run)
            'y.Start()
            Console.WriteLine("Finished raskroy for " & element.Color & ", dsp " & element.dsp & " ���-�� ������ = " & element.Rashod & " sum(scrap) = " & SumScrap / 1000000)
        Next
        Console.WriteLine("Press enter to exit...")
        Console.ReadLine()
    End Sub

End Module
