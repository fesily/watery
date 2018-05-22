using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using EnvDTE;
using Microsoft.VisualStudio.VCCodeModel;

namespace ReflectionPluginVSIX
{
    internal class ReflectionImpl
    {
        public static void CalcReflx(DTE dte)
        {
            //var textView = session.TextView;
            //var lineText = textView.Selection.ActivePoint.Position.GetContainingLine().Extent.GetText();
            Document activeDocument = dte.ActiveDocument;
            TextSelection textSelection = activeDocument.Selection as TextSelection;
            if (textSelection == null) return;
            VirtualPoint activePoint = textSelection.ActivePoint;
            var editPoint = activePoint.CreateEditPoint();
            editPoint.StartOfLine();
            var lineText = editPoint.GetText(activePoint.LineCharOffset);
            var text = lineText.Trim();
            var pattenBegin = "reflex(";
            var pattenEnd = ")";

            if (!text.StartsWith(pattenBegin) || !text.EndsWith(pattenEnd)) return;

            if (!(activeDocument.ProjectItem.FileCodeModel is VCFileCodeModel fileCodeModel)) return;

            text = text.Substring(pattenBegin.Length);
            text = text.Remove(text.Length - pattenEnd.Length);
            VCCodeElement[] codeElements;
            {
                var vcCodeElements = (fileCodeModel.CodeElementFromFullName(text) as VCCodeElements);
                if (vcCodeElements == null || vcCodeElements.Count == 0) return;
                codeElements = vcCodeElements.Cast<VCCodeElement>().ToArray();
            }
            BEGIN_CAST:
            if (codeElements.Length == 0) return;
            if (codeElements.Length != 1)
            {
                List<VCCodeElement> finders = new List<VCCodeElement>();
                bool maybeClassOrStruct = text.Contains("::");

                foreach (VCCodeElement element in codeElements)
                {
                    VCCodeElement parent = element;
                    // 根据情况,往上查找几层
                    for (int i = maybeClassOrStruct ? 0 : 1; i < 2; i++)
                    {
                        parent = parent.Parent as VCCodeElement;
                        if (parent == null)
                            break;
                        if (parent.StartPoint.LessThan(activePoint) && parent.EndPoint.GreaterThan(activePoint))
                        {
                            finders.Add(element);
                        }
                    }
                }
                if (finders.Count == 0)
                {
                    return;
                }
                codeElements = finders.ToArray();
            }
            VCCodeElement curCodeElement = codeElements[0];
            if (curCodeElement == null || curCodeElement.Kind == vsCMElement.vsCMElementNamespace)
                return;
            if (curCodeElement.Kind == vsCMElement.vsCMElementTypeDef ||
                curCodeElement.Kind == (vsCMElement)2000)
            {
                if (codeElements.Length > 1) return;

                if (curCodeElement is VCCodeTypedef codeTypedef)
                {
                    codeElements = fileCodeModel.CodeElementFromFullName(codeTypedef
                        .TypeString).Cast<VCCodeElement>().ToArray();
                    goto BEGIN_CAST;
                }
            }

            var reflexHeader = GetComplierTextHeader(codeElements.Length, curCodeElement);
            if (reflexHeader == null) return;
            StringBuilder allComplerText = null;
            foreach (VCCodeElement codeElement in codeElements)
            {
                var complierText = ComplierText(reflexHeader, text, codeElement);
                if (complierText == null)
                    continue;
                if (allComplerText == null)
                {
                    allComplerText = complierText;
                }
                else
                {
                    allComplerText.Append("\r\n" + complierText);
                }
            }

            if (allComplerText != null)
                activeDocument.ReplaceText(lineText, allComplerText.ToString(),
                    (int)vsFindOptions.vsFindOptionsMatchWholeWord);
        }
        private static StringBuilder ComplierText(string reflexHeader, string text, VCCodeElement codeElement)
        {
            var complierText = new StringBuilder(reflexHeader + '(' + text + ',');
            switch (codeElement.Kind)
            {
                case vsCMElement.vsCMElementEnum:
                    var codeEnum = codeElement as VCCodeEnum;
                    foreach (VCCodeElement enumMember in codeEnum.Members)
                    {
                        complierText.Append(enumMember.Name + ',');
                    }
                    complierText.Remove(complierText.Length - 1, 1);
                    break;
                case vsCMElement.vsCMElementClass:
                    if (codeElement is VCCodeClass codeClass)
                    {
                        if (codeClass.IsTemplate)
                        {
                            complierText.Append('(');
                            foreach (VCCodeParameter parameter in codeClass.TemplateParameters)
                            {
                                complierText.Append(parameter.DisplayName + ',');
                            }
                            complierText[complierText.Length - 1] = ')';
                            complierText.Append(',');
                        }
                        if (GetReflxString(codeClass.Functions as VCCodeElements,
                            codeClass.Variables as VCCodeElements,
                            complierText))
                        {
                            return null;
                        }
                    }
                    break;
                case vsCMElement.vsCMElementStruct:
                    if (codeElement is VCCodeStruct codeStruct)
                    {
                        if (codeStruct.IsTemplate)
                        {
                            complierText.Append('(');
                            foreach (VCCodeParameter parameter in codeStruct.TemplateParameters)
                            {
                                complierText.Append(parameter.DisplayName + ',');
                            }
                            complierText[complierText.Length - 1] = ')';
                            complierText.Append(',');
                        }

                        if (GetReflxString(codeStruct.Functions as VCCodeElements,
                            codeStruct.Variables as VCCodeElements,
                            complierText))
                        {
                            return null;
                        }
                    }
                    break;
                default:
                    return null;
            }
            complierText.Append(");");
            return complierText;
        }

        private static string GetComplierTextHeader(int codeElementCount, VCCodeElement codeElement)
        {
            bool isTemplate = false;
            if (codeElement == null) return null;
            string reflexHeader = null;
            switch (codeElement.Kind)
            {
                case vsCMElement.vsCMElementClass:
                case vsCMElement.vsCMElementStruct:
                    if ((codeElement is VCCodeClass codeClass && codeClass.IsTemplate)
                        || (codeElement is VCCodeStruct codeStruct && codeStruct.IsTemplate))
                    {
                        isTemplate = true;
                        reflexHeader = "WATERY_REFLECTION_TEMPALTE";
                    }
                    else
                    {
                        reflexHeader = "WATERY_REFLECTION";
                    }

                    break;
                case vsCMElement.vsCMElementEnum:
                    reflexHeader = "WATERY_REFLECTION_ENUM";
                    break;
                default: return null;
            }

            if (codeElementCount > 1 && !isTemplate)
            {
                // 只有template 允许对应多个同名声明
                return null;
            }

            return reflexHeader;
        }
        private static bool IsInvaildFunction(VCCodeFunction function)
        {
            return function.IsTemplate ||
                   function.Access != vsCMAccess.vsCMAccessPublic ||
                   function.FunctionKind.HasFlag(vsCMFunction.vsCMFunctionConstructor) ||
                   function.FunctionKind.HasFlag(vsCMFunction.vsCMFunctionDestructor) ||
                   function.FunctionKind.HasFlag(vsCMFunction.vsCMFunctionOperator);
        }

        private static bool GetReflxString(VCCodeElements functions, VCCodeElements variables,
            StringBuilder complierText)
        {
            StringBuilder overloadBuilder = new StringBuilder(), normalBuilder = new StringBuilder("(");
            HashSet<string> overloadHashSet = new HashSet<string>();
            bool isEmpty = true;
            foreach (VCCodeFunction function in functions)
            {
                if (IsInvaildFunction(function))
                {
                    continue;
                }
                isEmpty = false;
                if (function.IsOverloaded)
                {
                    if (!overloadHashSet.Contains(function.Name))
                    {
                        ComplierOverloadFunctionText(overloadBuilder, function);
                        overloadBuilder.Append(',');
                        overloadHashSet.Add(function.Name);
                    }
                }
                else
                {
                    ComplierFunctionText(normalBuilder, function);
                }
            }
            foreach (VCCodeVariable variable in variables)
            {
                if (variable.Access != vsCMAccess.vsCMAccessPublic) continue;
                isEmpty = false;
                normalBuilder.Append(variable.Name + ',');
            }

            normalBuilder[normalBuilder.Length - 1] = ')';
            complierText.Append(overloadBuilder.ToString() + normalBuilder.ToString());
            return isEmpty;
        }
        private static void ComplierFunctionText(StringBuilder nomalBuilder, VCCodeFunction function)
        {
            if (!function.FunctionKind.HasFlag(vsCMFunction.vsCMFunctionConstructor)
                && !function.FunctionKind.HasFlag(vsCMFunction.vsCMFunctionDestructor)
                && !function.FunctionKind.HasFlag(vsCMFunction.vsCMFunctionOperator))

            {
                nomalBuilder.Append(function.Name + ',');
            }
        }
        private static void ComplierOverloadFunctionText(StringBuilder overloadBuilder, VCCodeFunction function)
        {
            overloadBuilder.Append('(' + function.Name + ',');
            foreach (VCCodeFunction functionOverload in function.Overloads)
            {
                if (IsInvaildFunction(functionOverload))
                {
                    continue;
                }
                overloadBuilder.Append(functionOverload.TypeString + "(");

                if (functionOverload.Parameters.Count == 0)
                {
                    overloadBuilder.Append(')');
                }
                else
                {
                    foreach (VCCodeParameter parameter in functionOverload.Parameters)
                        overloadBuilder.Append(parameter.TypeString + ',');

                    overloadBuilder[overloadBuilder.Length - 1] = ')';
                }

                overloadBuilder.Append(',');
            }
            overloadBuilder[overloadBuilder.Length - 1] = ')';
        }
    }
}