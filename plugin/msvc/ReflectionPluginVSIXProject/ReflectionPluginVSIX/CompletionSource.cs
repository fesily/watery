using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using EnvDTE;
using Microsoft.VisualStudio.Language.Intellisense;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Text;
using Microsoft.VisualStudio.Utilities;

namespace ReflectionPluginVSIX
{
    [Export(typeof(ICompletionSourceProvider))]
    [ContentType("C/C++")]
    [Name("reflection token completion")]
    [Order(After = "default")]
    internal class ReflectionCompletionSourceProvider : ICompletionSourceProvider
    {
        [Import]
        internal SVsServiceProvider ServiceProvider { get; set; }
        public ICompletionSource TryCreateCompletionSource(ITextBuffer textBuffer)
        {
            try
            {
                return new ReflectionCompletionSource(this);

            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return null;
            }
        }
    }

    internal class ReflectionCompletionSource : ICompletionSource
    {
        private readonly ReflectionCompletionSourceProvider _sourceProvider;

        public ReflectionCompletionSource(ReflectionCompletionSourceProvider sourceProvider)
        {
            _sourceProvider = sourceProvider;
        }

        void OnSessionDismissed(object sender, EventArgs e)
        {
            var session = sender as ICompletionSession;
            if (session?.SelectedCompletionSet == null)
            {
                return;
            }
            if (_sourceProvider.ServiceProvider.GetService(typeof(DTE)) is DTE dte)
            {
                try
                {
                    ReflectionImpl.CalcReflx(dte);
                }
                catch (Exception ec)
                {
                    Console.WriteLine(ec);
                }
            }
            session.Dismissed -= OnSessionDismissed;
        }

        void OnSessionCommitted(object sender, EventArgs e)
        {
            var session = sender as ICompletionSession;
            if (session == null)
            {
                return;
            }

            session.Dismissed -= OnSessionDismissed;
        }
        void ICompletionSource.AugmentCompletionSession(ICompletionSession session, IList<CompletionSet> completionSets)
        {
            session.Dismissed += OnSessionDismissed;
            session.Committed += OnSessionCommitted;
        }

        private bool _isDisposed;
        public void Dispose()
        {
            if (!_isDisposed)
            {
                GC.SuppressFinalize(this);
                _isDisposed = true;
            }
        }
    }

}